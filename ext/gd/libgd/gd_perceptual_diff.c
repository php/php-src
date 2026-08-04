#include "gd.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define GD_PERCEPTUAL_MAX_DELTA 35215.0

static uint8_t gd_alpha_to_opacity(int alpha)
{
    return (uint8_t)(((127 - alpha) * 255 + 63) / 127);
}

static uint8_t blend(uint8_t color, uint8_t alpha, uint8_t background)
{
    return (uint8_t)((color * alpha + background * (255 - alpha) + 127) / 255);
}

static double pixel_diff_yiq(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2)
{
    double dr = r1 - r2;
    double dg = g1 - g2;
    double db = b1 - b2;
    double y = dr * 0.29889531 + dg * 0.58662247 + db * 0.11448223;
    double i = dr * 0.59597799 - dg * 0.27417610 - db * 0.32180189;
    double q = dr * 0.21147017 - dg * 0.52261711 + db * 0.31114694;

    return 0.5053 * y * y + 0.299 * i * i + 0.1957 * q * q;
}

BGD_DECLARE(int) gdImagePerceptualDiff(gdImagePtr image1, gdImagePtr image2, double threshold,
                      const gdImagePerceptualDiffOptions *options, gdImagePtr *diff_image,
                      gdImagePerceptualDiffResult *result)
{
    gdImageDiffMode mode = GD_IMAGE_DIFF_OVERLAY;
    int highlight_color = gdTrueColorAlpha(255, 0, 0, 0);
    gdImagePtr diff = NULL;
    double maximum_delta = 0.0;
    double max_yiq_delta;
    int alpha_threshold;
    int x, y;

    if (diff_image != NULL) {
        *diff_image = NULL;
    }
    if (result != NULL) {
        result->pixels_changed = 0;
        result->maximum_delta = 0.0;
    }

    if (image1 == NULL || image2 == NULL || result == NULL ||
        !(threshold >= 0.0 && threshold <= 1.0) || image1->sx != image2->sx ||
        image1->sy != image2->sy) {
        return 0;
    }

    if (options != NULL) {
        mode = options->mode;
        highlight_color = options->highlight_color;
    }
    if (mode < GD_IMAGE_DIFF_NONE || mode > GD_IMAGE_DIFF_MASK) {
        return 0;
    }

    if (mode != GD_IMAGE_DIFF_NONE && diff_image != NULL) {
        diff = gdImageCreateTrueColor(image1->sx, image1->sy);
		if (diff == NULL) {
			return 0;
		}
		gdImageAlphaBlending(diff, 0);
		gdImageSaveAlpha(diff, 1);
    }

    max_yiq_delta = GD_PERCEPTUAL_MAX_DELTA * threshold * threshold;
    alpha_threshold = (int)(threshold * 127);

    for (y = 0; y < image1->sy; y++) {
        for (x = 0; x < image1->sx; x++) {
            int p1 = gdImageGetTrueColorPixel(image1, x, y);
            int p2 = gdImageGetTrueColorPixel(image2, x, y);
            double delta = 0.0;
            int is_diff = 0;

            if (p1 != p2) {
                uint8_t r1 = gdTrueColorGetRed(p1);
                uint8_t g1 = gdTrueColorGetGreen(p1);
                uint8_t b1 = gdTrueColorGetBlue(p1);
                int ga1 = gdTrueColorGetAlpha(p1);
                uint8_t r2 = gdTrueColorGetRed(p2);
                uint8_t g2 = gdTrueColorGetGreen(p2);
                uint8_t b2 = gdTrueColorGetBlue(p2);
                int ga2 = gdTrueColorGetAlpha(p2);
                int alpha_delta = abs(ga1 - ga2);

                delta = (double)alpha_delta / 127.0;
                if (alpha_delta > alpha_threshold) {
                    is_diff = 1;
                } else {
                    uint8_t a1 = gd_alpha_to_opacity(ga1);
                    uint8_t a2 = gd_alpha_to_opacity(ga2);
                    double yiq_delta = 0.0;

                    if (a1 == 0 && a2 == 0) {
                        delta = 0.0;
                    } else if (a1 > 250 && a2 > 250) {
                        yiq_delta = pixel_diff_yiq(r1, g1, b1, r2, g2, b2);
                    } else {
                        double delta_white = pixel_diff_yiq(blend(r1, a1, 255), blend(g1, a1, 255),
                                                            blend(b1, a1, 255), blend(r2, a2, 255),
                                                            blend(g2, a2, 255), blend(b2, a2, 255));
                        double delta_black =
                            pixel_diff_yiq(blend(r1, a1, 0), blend(g1, a1, 0), blend(b1, a1, 0),
                                           blend(r2, a2, 0), blend(g2, a2, 0), blend(b2, a2, 0));
                        yiq_delta = delta_white < delta_black ? delta_white : delta_black;
                    }
                    if (yiq_delta > 0.0) {
                        double color_delta = sqrt(yiq_delta / GD_PERCEPTUAL_MAX_DELTA);
                        if (color_delta > delta) {
                            delta = color_delta;
                        }
                        is_diff = yiq_delta > max_yiq_delta;
                    }
                }
            }

            if (delta > maximum_delta) {
                maximum_delta = delta;
            }
            if (is_diff) {
                result->pixels_changed++;
                if (diff != NULL) {
                    gdImageSetPixel(diff, x, y, highlight_color);
                }
            } else if (diff != NULL && mode == GD_IMAGE_DIFF_OVERLAY) {
                gdImageSetPixel(diff, x, y,
                                gdTrueColorAlpha(gdTrueColorGetRed(p1), gdTrueColorGetGreen(p1),
                                                 gdTrueColorGetBlue(p1), 102));
            } else if (diff != NULL) {
                gdImageSetPixel(diff, x, y, gdTrueColorAlpha(0, 0, 0, 127));
            }
        }
    }

    result->maximum_delta = maximum_delta;
    if (diff_image != NULL) {
        *diff_image = diff;
    }
    return 1;
}
