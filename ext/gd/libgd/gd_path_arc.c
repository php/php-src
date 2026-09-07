
/* This is a port of Carl's amazing work on this. The license chosen
 * here is the MPL 1.1, applying to this file only.
 *
 * Copyright © 2004 Red Hat, Inc
 *
 * This library is free software; you can redistribute it and/or
 * modify it either under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * (the "LGPL") or, at your option, under the terms of the Mozilla
 * Public License Version 1.1 (the "MPL"). If you do not alter this
 * notice, a recipient may use your version of this file under either
 * the MPL or the LGPL.
 *
 * You should have received a copy of the LGPL along with this library
 * in the file COPYING-LGPL-2.1; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA
 * You should have received a copy of the MPL along with this library
 * in the file COPYING-MPL-1.1
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY
 * OF ANY KIND, either express or implied. See the LGPL or the MPL for
 * the specific language governing rights and limitations.
 *
 * The Original Code is the cairo graphics library.
 *
 * The Initial Developer of the Original Code is University of Southern
 * California.
 *
 * Contributor(s):
 *	Kristian Høgsberg <krh@redhat.com>
 *	Carl Worth <cworth@cworth.org>
 */

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "gd_errors.h"
#include "gd_intern.h"
#include "gd_vector2d_private.h"
#include "gdhelpers.h"

#include "gd_array.h"
#include "gd_path.h"
#include "gd_path_arc.h"
#include "gd_path_matrix.h"
#include "gd_span_rle.h"
#include "gd_surface.h"

/* Spline deviation from the circle in radius would be given by:

        error = sqrt (x**2 + y**2) - 1

   A simpler error function to work with is:

        e = x**2 + y**2 - 1

   From "Good approximation of circles by curvature-continuous Bezier
   curves", Tor Dokken and Morten Daehlen, Computer Aided Geometric
   Design 8 (1990) 22-41, we learn:

        abs (max(e)) = 4/27 * sin**6(angle/4) / cos**2(angle/4)

   and
        abs (error) =~ 1/2 * e

   Of course, this error value applies only for the particular spline
   approximation that is used in _gd_arc_segment.

   Detailed explanation https://itc.ktu.lt/index.php/ITC/article/view/11812
*/
static double _arc_error_normalized(double angle)
{
    return 2.0 / 27.0 * pow(sin(angle / 4), 6) / pow(cos(angle / 4), 2);
}

static double _arc_max_angle_for_tolerance_normalized(double tolerance)
{
    double angle, error;
    int i;

    /* Use table lookup to reduce search time in most cases. */
    struct {
        double angle;
        double error;
    } table[] = {
        {M_PI / 1.0, 0.0185185185185185036127},    {M_PI / 2.0, 0.000272567143730179811158},
        {M_PI / 3.0, 2.38647043651461047433e-05},  {M_PI / 4.0, 4.2455377443222443279e-06},
        {M_PI / 5.0, 1.11281001494389081528e-06},  {M_PI / 6.0, 3.72662000942734705475e-07},
        {M_PI / 7.0, 1.47783685574284411325e-07},  {M_PI / 8.0, 6.63240432022601149057e-08},
        {M_PI / 9.0, 3.2715520137536980553e-08},   {M_PI / 10.0, 1.73863223499021216974e-08},
        {M_PI / 11.0, 9.81410988043554039085e-09},
    };
    int table_size = ARRAY_LENGTH(table);

    for (i = 0; i < table_size; i++)
        if (table[i].error < tolerance)
            return table[i].angle;

    ++i;
    do {
        angle = M_PI / i++;
        error = _arc_error_normalized(angle);
    } while (error > tolerance);

    return angle;
}

static int _arc_segments_needed(double angle, double radius, double tolerance)
{
    double major_axis, max_angle;

    major_axis = radius;
    max_angle = _arc_max_angle_for_tolerance_normalized(tolerance / major_axis);

    return ceil(fabs(angle) / max_angle);
}

/* We want to draw a single spline approximating a circular arc radius
   R from angle A to angle B. Since we want a symmetric spline that
   matches the endpoints of the arc in position and slope, we know
   that the spline control points must be:

        (R * cos(A), R * sin(A))
        (R * cos(A) - h * sin(A), R * sin(A) + h * cos (A))
        (R * cos(B) + h * sin(B), R * sin(B) - h * cos (B))
        (R * cos(B), R * sin(B))

   for some value of h.

   "Approximation of circular arcs by cubic polynomials", Michael
   Goldapp, Computer Aided Geometric Design 8 (1991) 227-238, provides
   various values of h along with error analysis for each.

   From that paper, a very practical value of h is:

        h = 4/3 * R * tan(angle/4)

   This value does not give the spline with minimal error, but it does
   provide a very good approximation, (6th-order convergence), and the
   error expression is quite simple, (see the comment for
   _arc_error_normalized).
*/
static void _gd_arc_segment(gdPathPtr path, double xc, double yc, double radius, double angle_A,
                            double angle_B)
{
    double r_sin_A, r_cos_A;
    double r_sin_B, r_cos_B;
    double h;

    r_sin_A = radius * sin(angle_A);
    r_cos_A = radius * cos(angle_A);
    r_sin_B = radius * sin(angle_B);
    r_cos_B = radius * cos(angle_B);

    h = 4.0 / 3.0 * tan((angle_B - angle_A) / 4.0);

    gdPathCurveTo(path, xc + r_cos_A - h * r_sin_A, yc + r_sin_A + h * r_cos_A,
                  xc + r_cos_B + h * r_sin_B, yc + r_sin_B - h * r_cos_B, xc + r_cos_B,
                  yc + r_sin_B);
}

static void _gd_arc_in_direction(gdPathPtr path, double xc, double yc, double radius,
                                 double angle_min, double angle_max, arcDirectionType dir)
{
    if (!path)
        return;

    if (angle_max - angle_min > 2 * M_PI * MAX_FULL_CIRCLES) {
        angle_max = fmod(angle_max - angle_min, 2 * M_PI);
        angle_min = fmod(angle_min, 2 * M_PI);
        angle_max += angle_min + 2 * M_PI * MAX_FULL_CIRCLES;
    }

    /* Recurse if drawing arc larger than pi */
    if (angle_max - angle_min > M_PI) {
        double angle_mid = angle_min + (angle_max - angle_min) / 2.0;
        if (dir == ARC_CW) {
            _gd_arc_in_direction(path, xc, yc, radius, angle_min, angle_mid, dir);

            _gd_arc_in_direction(path, xc, yc, radius, angle_mid, angle_max, dir);
        } else {
            _gd_arc_in_direction(path, xc, yc, radius, angle_mid, angle_max, dir);

            _gd_arc_in_direction(path, xc, yc, radius, angle_min, angle_mid, dir);
        }
    } else if (angle_max != angle_min) {
        int i, segments;
        double step;
        segments = _arc_segments_needed(angle_max - angle_min, radius, DEFAULT_TOLERANCE);
        step = (angle_max - angle_min) / segments;
        segments -= 1;

        if (dir == ARC_CCW) {
            double t;

            t = angle_min;
            angle_min = angle_max;
            angle_max = t;

            step = -step;
        }
        gdPathMoveTo(path, xc + radius * cos(angle_min), yc + radius * sin(angle_min));

        for (i = 0; i < segments; i++, angle_min += step) {
            _gd_arc_segment(path, xc, yc, radius, angle_min, angle_min + step);
        }

        _gd_arc_segment(path, xc, yc, radius, angle_min, angle_max);
    } else {
        gdPathLineTo(path, xc + radius * cos(angle_min), yc + radius * sin(angle_min));
    }
}

void _gd_arc_path(gdPathPtr path, double xc, double yc, double radius, double angle_min,
                  double angle_max)
{
    if (angle_min > M_PI2) {
        angle_min = fmod(angle_min, M_PI2);
    }

    if (angle_max > M_PI2) {
        printf(" fmod max -");
        angle_max = fmod(angle_max, M_PI2);
    }

    if (angle_max - angle_min > 2 * M_PI * MAX_FULL_CIRCLES) {
        angle_max = fmod(angle_max - angle_min, 2 * M_PI);
        angle_min = fmod(angle_min, 2 * M_PI);
        angle_max += angle_min + 2 * M_PI * MAX_FULL_CIRCLES;
    }
    _gd_arc_in_direction(path, xc, yc, radius, angle_min, angle_max, ARC_CW);
}

void _gd_arc_path_negative(gdPathPtr path, double xc, double yc, double radius, double angle1,
                           double angle2)
{
    _gd_arc_in_direction(path, xc, yc, radius, angle2, angle1, ARC_CCW);
}
