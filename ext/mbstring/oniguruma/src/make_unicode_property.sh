#!/bin/sh

GPERF=gperf
SED=sed

NAME=unicode_property_data
TMP1=gperf1.tmp
TMP2=gperf2.tmp
TMP=

GPERF_OPT='-T -C -c -t -j1 -L ANSI-C --ignore-case --pic -Q unicode_prop_name_pool'
POOL_CAST='s/\(int *\)\(size_t *\)&\(\(struct +unicode_prop_name_pool_t *\* *\) *0\)->unicode_prop_name_pool_str([^,]+)/pool_offset(\1)/g'
ADD_STATIC='s/(const +struct +PoolPropertyNameCtype +\*)/static \1/'
ADD_CAST='s/unsigned +int +hval *= *len/unsigned int hval = (unsigned int )len/'

python3 make_unicode_property_data.py > ${NAME}.gperf
python3 make_unicode_property_data.py -posix > ${NAME}_posix.gperf

${GPERF} ${GPERF_OPT} -N unicode_lookup_property_name --output-file ${TMP1} ${NAME}.gperf
cat ${TMP1} | ${SED} -e 's/^#line.*$//g' | ${SED} -E "${POOL_CAST}" | ${SED} -E "${ADD_STATIC}" | ${SED} -E "${ADD_CAST}" > ${NAME}.c

${GPERF} ${GPERF_OPT} -N unicode_lookup_property_name --output-file ${TMP2} ${NAME}_posix.gperf
cat ${TMP2} | ${SED} -e 's/^#line.*$//g' | ${SED} -E "${POOL_CAST}" | ${SED} -E "${ADD_STATIC}" > ${NAME}_posix.c

rm -f ${NAME}.gperf ${NAME}_posix.gperf ${TMP1} ${TMP2}

exit 0
