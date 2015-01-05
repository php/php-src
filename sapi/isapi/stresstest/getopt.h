/* Borrowed from Apache NT Port */
#ifdef __cplusplus
extern "C" {
#endif
extern char *ap_optarg;
extern int ap_optind;

int ap_getopt(int argc, char* const *argv, const char *optstr);

#ifdef __cplusplus
}
#endif