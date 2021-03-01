//
//  main.cpp
//  LAB
//
//  Created by 최종원 on 2021/01/20.
//
#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include "stb_image.h"
#include "stb_image_write.h"

#include "rtweekend.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "triangle.h"
#include "camera.h"
#include "material.h"

#include <igl/readOBJ.h>
#include <igl/opengl/glfw/Viewer.h>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "toys.h"

using namespace std;

// settings
const unsigned int WIDTH = 900;
const unsigned int HEIGHT = 600;
const unsigned int CHANNEL = 3;
unsigned int texture;
unsigned int VBO, VAO, EBO;
Program program;

void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void render(GLFWwindow *window, uint8_t* pixels){
    // input
    // ------
    processInput(window);

    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // bind Texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, texture);

    // render container
    glUseProgram(program.programID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
}

//double hit_sphere(const point3& center, double radius, const ray& r) {
//    vec3 oc = r.origin() - center;
//    auto a = r.direction().length_squared();
//    auto half_b = dot(oc, r.direction());
//    auto c = oc.length_squared() - radius*radius;
//    auto discriminant = half_b*half_b - a*c;
//
//    if (discriminant < 0) {
//        return -1.0;
//    } else {
//        return (-half_b - sqrt(discriminant) ) / a;
//    }
//}

std::vector<std::vector<double>>V, N, TC;
std::vector<std::vector<int>>F, FTC, FN;

vec3 toVec3( const std::vector<double>& v ) {
    return vec3(v[0],v[1],v[2]);
}

hittable_list random_scene() {
    hittable_list world;
    igl::readOBJ("/Users/choijongwon/Downloads/lowpolydeer/deer.obj", V, TC, N, F, FTC, FN);
    
    double tempVertex[3];
    point3 triangleVertex[3];
    vec3 triangleVertexNormal[3];
    vec3 vertexNormal[V.size()];
    
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    shared_ptr<material> triangle_material;
//    triangle_material = make_shared<lambertian>(color(0.4, 0.2, 0.2));
//    triangle_material = make_shared<dielectric>(1.5);
    triangle_material = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    
    for(int i = 0; i < V.size(); i++){
        vertexNormal[i] = vec3(0,0,0);
        for(int j = 0; j < F.size(); j++){
        
            bool vertexCheck = false;
            int order = 0;
            for(auto iter = F[j].begin(); iter != F[j].end(); iter++){
                if(*iter == i){
                    vertexCheck = true;
                    break;
                }
                order++;
            }
            
            vec3 tempNormal;
            if(!vertexCheck) continue;
            vec3 v0v1 = toVec3(V[F[j][(order + 1) % 3]]) - toVec3(V[F[j][order]]);
            vec3 v0v2 = toVec3(V[F[j][(order + 2) % 3]]) - toVec3(V[F[j][order]]);
            tempNormal = cross(v0v1, v0v2);
            vertexNormal[i] += tempNormal;
//                cout << " V[" << i << "]는 F["<< j <<"]의 " << order << " 번째 정점에 속해있음" << endl;
        }
        vertexNormal[i] = unit_vector(vertexNormal[i]);
//        cout << "V[" << i << "]의 노말벡터는 " << vertexNormal[i] << endl;
        cout << endl;
    }
    
    for(int i = 0; i < F.size(); i++){
        int k = 0;
        for(auto iter = F[i].begin(); iter != F[i].end(); iter++){
            int j = 0;
            for(auto iter2 = V[*iter].begin(); iter2 != V[*iter].end(); iter2++){
                tempVertex[j] = *iter2 * 0.003;
                j++;
            }
            
            triangleVertexNormal[k] = vertexNormal[*iter];
//            cout << i << " 번째 Face에서 " << *iter << " 번 Vertex의 normal vector는 " << triangleVertexNormal[k] << endl;
            triangleVertex[k] = point3(tempVertex[0], tempVertex[1], tempVertex[2]);
            k++;
        }
        world.add(make_shared<triangle>(triangleVertex[0], triangleVertex[1], triangleVertex[2], triangleVertexNormal[0], triangleVertexNormal[1], triangleVertexNormal[2], triangle_material));
    }

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }
    return world;
}

color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,1);// 확인용 원래는 (0,0,0)

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
//            return (rec.front_face ? attenuation : color(0, 1, 0)) * ray_color(scattered, world, depth-1);
            return attenuation * ray_color(scattered, world, depth-1);
        return color(1,0,0);// 확인용 원래는 (0,0,0)
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0); // -1~1 -> 0~1
    // “linear blend” or “linear interpolation” or “lerp” ->blendedValue=(1−t)*startValue + t*endValue
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

void ray_calculate(int start, int end, int height, int width, int samples_per_pixel, int max_depth, camera cam, hittable_list world, uint8_t* pixels){
    for (int j = start; j < end; j++) {
        for (int i = 0; i < width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (width-1);
                auto v = (j + random_double()) / (height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            
            color color = write_color(pixel_color, samples_per_pixel);
        
            pixels[(i + j * width)*CHANNEL+0] = static_cast<int>(color.x());
            pixels[(i + j * width)*CHANNEL+1] = static_cast<int>(color.y());
            pixels[(i + j * width)*CHANNEL+2] = static_cast<int>(color.z());
        }
    }
}

int main() {
    
    // Image
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 900;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int thread_number = 24;
    const int height_interval = (image_height+thread_number-1)/thread_number;
//    const int samples_per_pixel = 100;
//    const int max_depth = 30;
    const int samples_per_pixel = 100;
    const int max_depth = 30;
    
    uint8_t* data = new uint8_t[WIDTH * HEIGHT * CHANNEL];
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ray Tracing", NULL, NULL);
    
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    program.loadShaders("shader.vert", "shader.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
        // positions          // colors           // texture coords
        1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

//    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // World
    auto world = random_scene();

    // Camera
//    point3 lookfrom(9,11,5);
    point3 lookfrom(7,9,7);

    point3 lookat(0,2,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.03;
    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    // Render
    thread t[thread_number];
    const int itv = height_interval;

    for(int i = 0;i < thread_number; i++){
        t[i] = thread(ray_calculate, itv*i, min(itv*(i+1),image_height), image_height, image_width, samples_per_pixel, max_depth, cam, world, data);
    }
    for(int i = 0;i < thread_number; i++){
        t[i].detach();
    }

//    stbi_write_png("/Users/choijongwon/image.png", image_width, image_height, CHANNEL, data, image_width * CHANNEL);
    
    while (!glfwWindowShouldClose(window)){
        render(window, data);
    }
    
    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    
    std::cerr << "\nDone.\n";
}



