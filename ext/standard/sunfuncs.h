#ifndef SUNFUNCS_H
#define SUNFUNCS_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define to_rad(degrees) (degrees * M_PI / 180)
#define to_rad_with_min(degrees) (degrees + minutes / 60)
#define to_deg(rad) (rad * 180 / M_PI)


/* default ini entries: */
/* Jerusalem one. */
#define DATE_DEFAULT_LATITUDE "31.7667"
#define DATE_DEFAULT_LONGITUDE "35.2333"
/* on 90'50; common jewish sunset declaration (start of sun body appear) */
#define DATE_SUNSET_ZENITH "90.83"
/* on 90'50; common jewish  sunrise declaration (sun body disappeared) */
#define DATE_SUNRISE_ZENITH "90.83"

#define SUNFUNCS_RET_TIMESTAMP 0
#define SUNFUNCS_RET_STRING 1
#define SUNFUNCS_RET_DOUBLE 2
double sunrise(long N, double latitude, double longitude, double zenith);
double sunset(long N, double latitude, double longitude, double zenith);

PHP_FUNCTION(date_sunrise);
PHP_FUNCTION(date_sunset);

#endif /* SUNFUNCS_H */
