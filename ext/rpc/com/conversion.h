#ifndef CONVERSION_H
#define CONVERSION_H

#ifdef __cplusplus
extern "C" {
#endif

void php_pval_to_variant(pval *pval_arg, VARIANT *var_arg);
void php_pval_to_variant_ex(pval *pval_arg, VARIANT *var_arg, pval *pval_type);
void php_variant_to_pval(VARIANT *var_arg, pval *pval_arg, int persistent);
OLECHAR *php_char_to_OLECHAR(char *C_str, uint strlen);
char *php_OLECHAR_to_char(OLECHAR *unicode_str, uint *out_length, int persistent);

#ifdef __cplusplus
}
#endif

#endif