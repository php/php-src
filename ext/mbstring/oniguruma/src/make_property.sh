#!/bin/sh

GPERF=gperf
SED=sed

TMP1=gperf1.tmp
TMP2=gperf2.tmp

GPERF_OPT='-pt -T -L ANSI-C'

ADD_CAST='s/return +len +\+ +asso_values/return (unsigned int )len + asso_values/'

${GPERF} ${GPERF_OPT} -N onigenc_euc_jp_lookup_property_name --output-file ${TMP1} euc_jp_prop.gperf
cat ${TMP1} | ${SED} -E "${ADD_CAST}" > euc_jp_prop.c

${GPERF} ${GPERF_OPT} -N onigenc_sjis_lookup_property_name --output-file ${TMP2} sjis_prop.gperf
cat ${TMP2} | ${SED} -E "${ADD_CAST}" > sjis_prop.c

rm -f ${TMP1} ${TMP2}

exit 0
