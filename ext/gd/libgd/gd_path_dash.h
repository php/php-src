#ifndef GD_PATH_DASH_H
#define GD_PATH_DASH_H


#define _dash_init(dash) \
    do { \
        dash.data = NULL; \
        dash.size = 0; \
        dash.capacity = 0; \
    } while(0)

#define _dash_allocate(dash, count) \
    do { \
    if(dash.size + count > dash.capacity) { \
        int capacity = dash.size + count; \
        int newcapacity = dash.capacity == 0 ? 8 : dash.capacity; \
        while(newcapacity < capacity) { newcapacity *= 2; } \
        dash.data = gdRealloc(dash.data, (size_t)newcapacity * sizeof(dash.data[0])); \
        dash.capacity = newcapacity; \
    } \
    } while(0)

gdPathDashPtr gdPathDashCreate(const double *data, int size, double offset);
gdPathDashPtr gdPathDashClone(const gdPathDashPtr dash);
void gdPathDashDestroy(gdPathDashPtr dash);
gdPathPtr gdPathApplyDash(const gdPathDashPtr dash, const gdPathPtr path);
#endif // GD_PATH_DASH_H
