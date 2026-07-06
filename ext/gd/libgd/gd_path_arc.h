#ifndef GD_PATH_ARC_H
#define GD_PATH_ARC_H
void _gd_arc_path(gdPathPtr path, double xc, double yc, double radius, double angle1,
                  double angle2);

void _gd_arc_path_negative(gdPathPtr path, double xc, double yc, double radius, double angle1,
                           double angle2);

typedef enum arcDirectionTypeStruct { ARC_CW, ARC_CCW } arcDirectionType;
#endif // GD_PATH_ARC_H
