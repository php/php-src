#include <ruby/ruby.h>
#include <ruby/encoding.h>
#include "regenc.h"

void
Init_gb2312(void)
{
    rb_enc_register("GB2312", rb_enc_find("EUC-KR"));
}

ENC_ALIAS("EUC-CN", "GB2312")
ENC_ALIAS("eucCN", "GB2312")
ENC_REPLICATE("GB12345", "GB2312")
