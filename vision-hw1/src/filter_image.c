#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    // TODO
    int i;
    float sum = 0;
    for (i = 0; i < im.c*im.h*im.w; ++i){
      sum += im.data[i];
    }
    if (!sum) return;
    for (i = 0; i < im.c*im.h*im.w; ++i){
      im.data[i] /= sum;
    }
}

image make_box_filter(int w)
{
    // TODO
    image box_filter = make_image(w, w, 1);
    int i;
    for (i = 0; i < w*w; ++i){
      box_filter.data[i] = 1.0 / (w*w);
    }
    return box_filter;
}

image convolve_image(image im, image filter, int preserve)
{
    // TODO
    assert(filter.c == 1 || filter.c == im.c);
    image im_out = make_image(im.w, im.h, preserve ? im.c : 1);
    int i, j, k, h_offset, w_offset, m, n;
    h_offset = filter.h / 2;
    w_offset = filter.w / 2;
    for (i = 0; i < im.c; ++i){
      for (j = 0 - h_offset; j < im.h - h_offset; ++j){
        for (k = 0 - w_offset; k < im.w - w_offset; ++k){
          float v = preserve ? 0 : get_pixel(im_out, k+w_offset, j+h_offset, 0);
          for (m = 0; m < filter.h; ++m){
            for (n = 0; n < filter.w; ++n){
              v += get_pixel(im, k + n, j + m, i) * \
                   get_pixel(filter, n, m, filter.c == 1 ? 0 : i);
            }
          }
          set_pixel(im_out, k+w_offset, j+h_offset, preserve ? i : 0, v);
        }
      }
    }
    return im_out;
}

image make_highpass_filter()
{
    // TODO
    image highpass_filter = make_image(3, 3, 1);
    float weight[9] = {0, -1, 0,
                       -1, 4, -1,
                       0, -1, 0};
    memcpy(highpass_filter.data, weight, sizeof(weight));
    return highpass_filter;
}

image make_sharpen_filter()
{
    // TODO
    image sharpen_filter = make_image(3, 3, 1);
    float weight[9] = {0, -1, 0,
                       -1, 5, -1,
                       0, -1, 0};
    memcpy(sharpen_filter.data, weight, sizeof(weight));
    return sharpen_filter;
}

image make_emboss_filter()
{
    // TODO
    image emboss_filter = make_image(3, 3, 1);
    float weight[9] = {-2, -1, 0,
                       -1, 1, 1,
                       0, 1, 2};
    memcpy(emboss_filter.data, weight, sizeof(weight));
    return emboss_filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: box_filter: set preserve to '1', because we just want to smooth the
//         image.
//         Sum the weights of highpass_filter, sharpen_filter and emboss_filter.
//         highpass_filter gives a '0' summation, while sharpen_filter and
//         emboss_filter gives a '1'
//         highpass_filter asks for derivatives and gives '0' summation, so the
//         preserve should be set to '0'.
//         sharpen_filter and emboss_filter can produce some stylish rgb image
//         when preserve is set to '1'.
//         preserve = 0 also makes sense for sharpen_filter and emboss_filter,
//         but not the common usage.
// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: After using highpass_filter, sharpen_filter or emboss_filter, we need
//         to do the post-processing (clamp_image() or feature_normalize()) to
//         make sure all the pixel values are valid(between 0 and 1).
//         No post-processing is required for box_filter, because all the pixel
//         values should be valid, theoretically. In ./src/test.c, we do
//         clamp_image() after using box_filter, comment that line and test
//         again, nothing will change.

image make_gaussian_filter(float sigma)
{
    // TODO
    int i, j, offset;
    int w = 6 * sigma;
    w = w > 0 ? ((w % 2 == 1) ? w : w + 1) : 1;
    image gaussian_filter = make_image(w, w, 1);
    offset = w / 2;
    for (i = 0 - offset; i < w - offset; ++i){
      for (j = 0 - offset; j < w - offset; ++j){
        float v = 1 / (TWOPI*sigma*sigma) * expf((- j*j - i*i) / (2*sigma*sigma));
        set_pixel(gaussian_filter, j+offset, i+offset, 0, v);
      }
    }
    return gaussian_filter;
}

image add_image(image a, image b)
{
    // TODO
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image im_out = make_image(a.w, a.h, a.c);
    int i;
    for (i = 0; i < a.c*a.h*a.w; ++i){
      im_out.data[i] = a.data[i] + b.data[i];
    }
    return im_out;
}

image sub_image(image a, image b)
{
    // TODO
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image im_out = make_image(a.w, a.h, a.c);
    int i;
    for (i = 0; i < a.c*a.h*a.w; ++i){
      im_out.data[i] = a.data[i] - b.data[i];
    }
    return im_out;
}

image make_gx_filter()
{
    // TODO
    image gx_filter = make_image(3, 3, 1);
    float weight[9] = {-1, 0, 1,
                       -2, 0, 2,
                       -1, 0, 1};
    memcpy(gx_filter.data, weight, sizeof(weight));
    return gx_filter;
}

image make_gy_filter()
{
    // TODO
    image gy_filter = make_image(3, 3, 1);
    float weight[9] = {-1, -2, -1,
                       0, 0, 0,
                       1, 2, 1};
    memcpy(gy_filter.data, weight, sizeof(weight));
    return gy_filter;
}

void feature_normalize(image im)
{
    // TODO
    int i;
    float min = 1, max = 0;
    float range;
    for (i = 0; i < im.c*im.h*im.w; ++i){
      if (im.data[i] > max) max = im.data[i];
      if (im.data[i] < min) min = im.data[i];
    }
    range = max - min;
    if (!range){
      for (i = 0; i < im.c*im.h*im.w; ++i){
        im.data[i] = 0;
      }
    } else{
      for (i = 0; i < im.c*im.h*im.w; ++i){
        im.data[i] = (im.data[i] - min) / range;
      }
    }
}

image *sobel_image(image im)
{
    // TODO
    int i = 0;
    image *res = calloc(2, sizeof(image));
    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();
    image gx = convolve_image(im, gx_filter, 0);
    image gy = convolve_image(im, gy_filter, 0);
    image mag = make_image(gx.w, gx.h, 1);
    image theta = make_image(gx.w, gx.h, 1);
    for (i = 0; i < im.h*im.w; ++i){
      mag.data[i] = sqrtf(gx.data[i]*gx.data[i] + gy.data[i]*gy.data[i]);
      theta.data[i] = atan2f(gy.data[i], gx.data[i]);
    }
    res[0] = mag;
    res[1] = theta;
    free_image(gx_filter);
    free_image(gy_filter);
    free_image(gx);
    free_image(gy);
    return res;
}

image colorize_sobel(image im)
{
    // TODO
    image colorized = make_image(im.w, im.h, 3);
    image *res = sobel_image(im);
    feature_normalize(res[0]);
    feature_normalize(res[1]);
    memcpy(colorized.data,             res[1].data, im.h*im.w*sizeof(float));
    memcpy(colorized.data+im.h*im.w,   res[0].data, im.h*im.w*sizeof(float));
    memcpy(colorized.data+2*im.h*im.w, res[0].data, im.h*im.w*sizeof(float));
    hsv_to_rgb(colorized);
    free_image(res[0]);
    free_image(res[1]);
    return colorized;
}
