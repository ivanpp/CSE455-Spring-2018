#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    // TODO Fill in
    return get_pixel(im, roundf(x), roundf(y), c);
}

image nn_resize(image im, int w, int h)
{
    // TODO Fill in (also fix that first line)
    image im_nn = make_image(w, h, im.c);
    int i, j, k;
    float w_scale = (float)im.w / w;
    float h_scale = (float)im.h / h;
    for (i = 0; i < im.c; ++i){
      for (j = 0; j < h; ++j){
        for (k = 0; k < w; ++k){
          float w_mapped = w_scale * (k + 0.5) - 0.5;
          float h_mapped = h_scale * (j + 0.5) - 0.5;
          float v = nn_interpolate(im, w_mapped, h_mapped, i);
          set_pixel(im_nn, k, j, i, v);
        }
      }
    }
    return im_nn;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // TODO
    float v1, v2, v3, v4, q1, q2, q;
    v1 = get_pixel(im, floorf(x), floorf(y), c);
    v2 = get_pixel(im, ceilf(x), floorf(y), c);
    v3 = get_pixel(im, floorf(x), ceilf(y), c);
    v4 = get_pixel(im, ceilf(x), ceilf(y), c);
    q1 = v2 * (x - floorf(x)) + v1 * (ceilf(x) - x);
    q1 = floorf(x) == ceilf(x) ? v1 : q1;
    q2 = v4 * (x - floorf(x)) + v3 * (ceilf(x) - x);
    q2 = floorf(x) == ceilf(x) ? v3 : q2;
    q = q2 * (y - floorf(y)) + q1 * (ceilf(y) - y);
    q = floorf(y) == ceilf(y) ? q1 : q;
    return q;
}

image bilinear_resize(image im, int w, int h)
{
    // TODO
    image im_bilinear = make_image(w, h, im.c);
    int i, j, k;
    float w_scale = (float)im.w / w;
    float h_scale = (float)im.h / h;
    for (i = 0; i < im.c; ++i){
      for (j = 0; j < h; ++j){
        for (k = 0; k < w; ++k){
          float w_mapped = w_scale * (k + 0.5) - 0.5;
          float h_mapped = h_scale * (j + 0.5) - 0.5;
          float v = bilinear_interpolate(im, w_mapped, h_mapped, i);
          set_pixel(im_bilinear, k, j, i, v);
        }
      }
    }
    return im_bilinear;
}
