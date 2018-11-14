#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    // TODO Fill this in
    if (x < 0) x = 0;
    else if (x >= im.w) x = im.w - 1;
    if (y < 0) y = 0;
    else if (y >= im.h) y = im.h - 1;
    if (c >= 0 && c < im.c){
      return im.data[c*im.h*im.w + y*im.w + x];
    }
    else return 0;
}

void set_pixel(image im, int x, int y, int c, float v)
{
    // TODO Fill this in
    if (x >= 0 && x < im.w && y >= 0 && y < im.h && c >= 0 && c < im.c){
      im.data[c*im.h*im.w + y*im.w + x] = v;
    }
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    // TODO Fill this in
    memcpy(copy.data, im.data, im.c*im.h*im.w*sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    // TODO Fill this in
    int i, j;
    float scale[] = {0.299, 0.587, 0.114};
    for (i = 0; i < im.c; ++i){
      for (j = 0; j < im.h*im.w; ++j){
        gray.data[j] += scale[i] * im.data[i*im.h*im.w+j];
      }
    }
    return gray;
}

void scale_image(image im, int c, float v)
{
    int i;
    for (i = 0; i < im.h*im.w; ++i){
      im.data[c*im.h*im.w+i] *= v;
    }
}

void shift_image(image im, int c, float v)
{
    // TODO Fill this in
    int i;
    for (i = 0; i < im.h*im.w; ++i){
      im.data[c*im.h*im.w+i] += v;
    }
}

void clamp_image(image im)
{
    // TODO Fill this in
    int i;
    for (i = 0; i < im.c*im.h*im.w; ++i){
      im.data[i] = (im.data[i] > 1) ? 1 : ((im.data[i] < 0) ? 0 : im.data[i]);
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    // TODO Fill this in
    float r, g, b, h, s, v;
    float m, c;
    int i;
    for (i = 0; i < im.h*im.w; ++i){
      r = im.data[i];
      g = im.data[i + im.h * im.w];
      b = im.data[i + 2 * im.h * im.w];
      v = three_way_max(r, g, b);
      m = three_way_min(r, g, b);
      c = v - m;
      if (v == 0) s = 0;
      else s = c / v;
      if (v == m) h = 0;
      else{
        if (v == r) h = (g - b) / c;
        else if (v == g) h = (b - r) / c + 2;
        else h = (r - g) / c + 4;
      }
      if (h < 0) h = h / 6 + 1;
      else h /= 6;
      im.data[i] = h;
      im.data[i + im.h*im.w] = s;
      im.data[i + 2*im.h*im.w] = v;
    }

}

void hsv_to_rgb(image im)
{
    // TODO Fill this in
    int i;
    float h, s, v, r, g, b;
    float dst, mid_pos, mid_neg, m;
    for (i = 0; i < im.h*im.w; ++i){
      h = im.data[i];
      s = im.data[i + im.h*im.w];
      v = im.data[i + 2*im.h*im.w];  // max(r, g, b)
      h *= 6;
      m = v - s * v;  // min(r, g, b)
      dst = h - floor(h);
      mid_pos = s * v * dst + m;  // middle(r, g, b)
      mid_neg = s * v * (1 - dst) + m;  // middle(r, g, b)
      if (h >= 0 && h < 1){  // r, g, b
        r = v;
        g = mid_pos;
        b = m;
      } else if (h >= 1 && h < 2){  // g, r, b
        g = v;
        r = mid_neg;
        b = m;
      } else if (h >= 2 && h < 3){  // g, b, r
        g = v;
        b = mid_pos;
        r = m;
      } else if (h >= 3 && h < 4){  // b, g, r
        b = v;
        g = mid_neg;
        r = m;
      } else if (h >= 4 && h < 5){  // b, r, g
        b = v;
        r = mid_pos;
        g = m;
      } else{  // r, b, g
        r = v;
        b = mid_neg;
        g = m;
      }
      im.data[i] = r;
      im.data[i + im.h*im.w] = g;
      im.data[i + 2*im.h*im.w] = b;
    }
}

void rgb_to_hcl(image im)
{
    assert(im.c == 3);
    int i, j, k;
    image rgb = make_image(im.w, im.h, im.c);
    image xyz = make_image(im.w, im.h, im.c);
    image luv = make_image(im.w, im.h, im.c);
    // sRGB -> RGB (gamma decompression)
    for (i = 0; i < im.c*im.h*im.w; ++i){
      rgb.data[i] = im.data[i] > 0.04045 ? powf((im.data[i]+0.055)/1.055, 2.4) :
                                           im.data[i] / 12.92;
    }
    // RGB -> XYZ
    float scale[3][3] = {{0.4124, 0.3576, 0.1805},
                         {0.2126, 0.7152, 0.0722},
                         {0.0193, 0.1192, 0.9505}};
    for (i = 0; i < im.c; ++i){
      for (j = 0; j < im.c; ++j){
        for (k = 0; k < im.h*im.w; ++k){
          xyz.data[i*im.h*im.w+k] += scale[i][j] * rgb.data[j*im.h*im.w+k];
        }
      }
    }
    // XYZ -> Luv
    float un = 0.2009, vn = 0.4610, yn = 1.0;  // D65 white point
    for (i = 0; i < im.h*im.w; ++i){
      float yr = xyz.data[im.h*im.w+i] / yn;
      if (yr > powf(6.0/29, 3)){
        luv.data[i] = 116 * powf(yr, 1.0/3) - 16;
      } else{
        luv.data[i] = powf(29.0/3, 3) * yr;
      }
      float den = xyz.data[i] + 15*xyz.data[im.h*im.w+i] + 3*xyz.data[2*im.h*im.w+i];
      float u = 4 * xyz.data[i] / den;
      float v = 9 * xyz.data[im.h*im.w+i] / den;
      luv.data[im.h*im.w+i] = 13 * luv.data[i] * (u-un);
      luv.data[2*im.h*im.w+i] = 13 * luv.data[i] * (v-vn);
    }
    // Luv -> HCL
    for (i = 0; i < im.h*im.w; ++i){
      float u = luv.data[im.h*im.w+i];
      float v = luv.data[2*im.h*im.w+i];
      im.data[i] = atan2f(v, u);
      im.data[im.h*im.w+i] = sqrtf(u*u + v*v);
    }
    memcpy(im.data+2*im.h*im.w, luv.data, im.h*im.w*sizeof(float));
    free_image(rgb);
    free_image(xyz);
    free_image(luv);
}

void hcl_to_rgb(image im)
{
    assert(im.c == 3);
    int i, j, k;
    image luv = make_image(im.w, im.h, im.c);
    image xyz = make_image(im.w, im.h, im.c);
    image rgb = make_image(im.w, im.h, im.c);
    // HCL -> Luv
    for (i = 0; i < im.h*im.w; ++i){
      float h = im.data[i];
      float c = im.data[im.h*im.w+i];
      luv.data[im.h*im.w+i] = cosf(h) * c;
      luv.data[2*im.h*im.w+i] = sinf(h) * c;
    }
    memcpy(luv.data, im.data+2*im.h*im.w, im.h*im.w*sizeof(float));
    // Luv -> XYZ
    float un = 0.2009, vn = 0.4610, yn = 1.0;  // D65 white point
    for (i = 0; i < im.h*im.w; ++i){
      float u, v;
      if (!luv.data[i]){
        xyz.data[i] = 0.0;
        xyz.data[im.h*im.w+i] = 0.0;
        xyz.data[2*im.h*im.w+i] = 0.0;
        continue;
      }
      u = luv.data[im.h*im.w+i] / (13*luv.data[i]) + un;
      v = luv.data[2*im.h*im.w+i] / (13*luv.data[i]) + vn;
      if (luv.data[i] > 8){
        xyz.data[im.h*im.w+i] = yn * powf((luv.data[i]+16)/116, 3);
      } else{
        xyz.data[im.h*im.w+i] = yn * luv.data[i] * powf(3.0/29, 3);
      }
      xyz.data[i] = (9.0/4) * xyz.data[im.h*im.w+i] * u / v;
      xyz.data[2*im.h*im.w+i] = xyz.data[im.h*im.w+i] * (12-3*u-20*v) / (4*v);
    }
    // XYZ -> RGB
    float scale[3][3] = {{ 3.2406, -1.5372, -0.4986},
                         {-0.9689,  1.8758,  0.0415},
                         { 0.0557, -0.2040,  1.0570}};
    for (i = 0; i < im.c; ++i){
      for (j = 0; j < im.c; ++j){
        for (k = 0; k < im.h*im.w; ++k){
          rgb.data[i*im.h*im.w+k] += scale[i][j] * xyz.data[j*im.h*im.w+k];
        }
      }
    }
    // RGB -> sRGB
    for (i = 0; i < im.c*im.h*im.w; ++i){
      if (rgb.data[i] > 0.0031308){
        im.data[i] = (1+0.055) * powf(rgb.data[i], 1.0/2.4) - 0.055;
      } else{
        im.data[i] = 12.92 * rgb.data[i];
      }
    }
    free_image(luv);
    free_image(xyz);
    free_image(rgb);
}
