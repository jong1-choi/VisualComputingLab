//
//  color.h
//  LAB
//
//  Created by 최종원 on 2021/01/21.
//

#ifndef color_h
#define color_h

#include "vec3.h"
#include <iostream>

color write_color(color pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);
    
    r = 256 * clamp(r, 0.0, 0.999);
    g = 256 * clamp(g, 0.0, 0.999);
    b = 256 * clamp(b, 0.0, 0.999);
    
    color result = vec3(r, g, b);

    return result;
}

#endif /* color_h */
