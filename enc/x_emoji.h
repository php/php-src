#include "regint.h"

/*
 * Name: UTF8-DoCoMo, SJIS-DoCoMo
 * Link: http://www.nttdocomo.co.jp/english/service/imode/make/content/pictograph/basic/index.html
 * Link: http://www.nttdocomo.co.jp/english/service/imode/make/content/pictograph/extention/index.html
 */
ENC_REPLICATE("UTF8-DoCoMo", "UTF-8")
ENC_REPLICATE("SJIS-DoCoMo", "Windows-31J")

/*
 * Name: UTF8-KDDI, SJIS-KDDI, ISO-2022-JP-KDDI
 * Link: http://www.au.kddi.com/ezfactory/tec/spec/img/typeD.pdf
 */
ENC_REPLICATE("UTF8-KDDI", "UTF-8")
ENC_REPLICATE("SJIS-KDDI", "Windows-31J")
ENC_REPLICATE("ISO-2022-JP-KDDI", "ISO-2022-JP")
ENC_REPLICATE("stateless-ISO-2022-JP-KDDI", "stateless-ISO-2022-JP")

/*
 * Name: UTF8-SoftBank, SJIS-SoftBank
 * Link: http://creation.mb.softbank.jp/web/web_pic_about.html
 * Link: http://www2.developers.softbankmobile.co.jp/dp/tool_dl/download.php?docid=120&companyid=
 */
ENC_REPLICATE("UTF8-SoftBank", "UTF-8")
ENC_REPLICATE("SJIS-SoftBank", "Windows-31J")
