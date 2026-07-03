#ifndef GD_SPAN_RLE_H
#define GD_SPAN_RLE_H

#define _rle_spans_init(rle_s) \
    do { \
        rle_s.data = NULL; \
        rle_s.size = 0; \
        rle_s.capacity = 0; \
    } while(0)

#define _rle_spans_allocate(rle_s, count) \
    do { \
    if(rle_s.size + count > rle_s.capacity) { \
        int capacity = rle_s.size + count; \
        int newcapacity = rle_s.capacity == 0 ? 8 : rle_s.capacity; \
        while(newcapacity < capacity) { newcapacity *= 2; } \
        rle_s.data = gdRealloc(rle_s.data, (size_t)newcapacity * sizeof(rle_s.data[0])); \
        rle_s.capacity = newcapacity; \
    } \
    } while(0)

gdSpanRlePtr gdSpanRleCreate();
gdSpanRlePtr gdSpanRleRetain(gdSpanRlePtr rle);
void gdSpanRleDestroy(gdSpanRlePtr rle);
void gdSpanRleClear(gdSpanRlePtr rle);
void gdSpanRleRasterize(gdSpanRlePtr rle, const gdPathPtr path, const gdPathMatrixPtr matrix, const gdRectFPtr clip, const gdStrokePtr stroke, gdFillRule winding);
void gdSpanRlePathClip(gdSpanRlePtr rle, const gdSpanRlePtr clip);
gdSpanRlePtr gdSpanRleClone(gdSpanRlePtr rle);
gdSpanRlePtr gdSpanHorizontalClip(const gdSpanRlePtr a, const gdSpanRlePtr b);
#endif // GD_SPAN_RLE_H
