#ifndef DATE_TMX_H
#define DATE_TMX_H

struct tmx_funcs {
    VALUE (*year)(void *dat);
    int (*yday)(void *dat);
    int (*mon)(void *dat);
    int (*mday)(void *dat);
    VALUE (*cwyear)(void *dat);
    int (*cweek)(void *dat);
    int (*cwday)(void *dat);
    int (*wnum0)(void *dat);
    int (*wnum1)(void *dat);
    int (*wday)(void *dat);
    int (*hour)(void *dat);
    int (*min)(void *dat);
    int (*sec)(void *dat);
    VALUE (*sec_fraction)(void *dat);
    VALUE (*secs)(void *dat);
    VALUE (*msecs)(void *dat);
    VALUE (*offset)(void *dat);
    char *(*zone)(void *dat);
};
struct tmx {
    void *dat;
    struct tmx_funcs *funcs;
};

#define tmx_attr(x) (tmx->funcs->x)(tmx->dat)

#define tmx_year tmx_attr(year)
#define tmx_yday tmx_attr(yday)
#define tmx_mon tmx_attr(mon)
#define tmx_mday tmx_attr(mday)
#define tmx_cwyear tmx_attr(cwyear)
#define tmx_cweek tmx_attr(cweek)
#define tmx_cwday tmx_attr(cwday)
#define tmx_wnum0 tmx_attr(wnum0)
#define tmx_wnum1 tmx_attr(wnum1)
#define tmx_wday tmx_attr(wday)
#define tmx_hour tmx_attr(hour)
#define tmx_min tmx_attr(min)
#define tmx_sec tmx_attr(sec)
#define tmx_sec_fraction tmx_attr(sec_fraction)
#define tmx_secs tmx_attr(secs)
#define tmx_msecs tmx_attr(msecs)
#define tmx_offset tmx_attr(offset)
#define tmx_zone tmx_attr(zone)

#endif

/*
Local variables:
c-file-style: "ruby"
End:
*/
