
#include "php.h"

#if HAVE_LIBMCRYPT

#include "php_mcrypt.h"

#include "lcrypt.h"

function_entry mcrypt_functions[] = {
	PHP_FE(mcrypt_ecb, NULL)
	{0},
};

static int php3_minit_mcrypt(INIT_FUNC_ARGS);

zend_module_entry mcrypt_module_entry = {
	"mcrypt", 
	mcrypt_functions,
	php3_minit_mcrypt, NULL,
	NULL, NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES,
};

#if 0

typedef struct mcrypt_global_struct {
	int le_h;
} mcrypt_global_struct;

static mcrypt_global_struct mcryptg;

#define MCRYPTG(x) mcryptg.x

#endif

#define MCRYPT_ENTRY(a) REGISTER_LONG_CONSTANT("MC_" #a, a, 0)

static int php3_minit_mcrypt(INIT_FUNC_ARGS)
{
	MCRYPT_ENTRY(BLOWFISH);
	MCRYPT_ENTRY(DES);
	MCRYPT_ENTRY(TripleDES);
	MCRYPT_ENTRY(ThreeWAY);
	MCRYPT_ENTRY(GOST);
	MCRYPT_ENTRY(SAFER64);
	MCRYPT_ENTRY(SAFER128);
	MCRYPT_ENTRY(CAST128);
	MCRYPT_ENTRY(TEAN);
	MCRYPT_ENTRY(TWOFISH);
	MCRYPT_ENTRY(RC2);
#ifdef CRYPT
	MCRYPT_ENTRY(CRYPT);
#endif
	return SUCCESS;
}

PHP_FUNCTION(mcrypt_ecb)
{
	pval *cipher, *data, *key, *mode;
	int td;
	char *ndata;
	size_t bsize;
	size_t nr;
	size_t nsize;

	if(ARG_COUNT(ht) != 4 || getParameters(ht, 4, &cipher, &key, &data, &mode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(cipher);
	convert_to_long(mode);
	convert_to_string(data);
	convert_to_string(key);

	bsize = get_block_size(cipher->value.lval);
	nr = (data->value.str.len + bsize - 1) / bsize;
	nsize = nr * bsize;

	td = init_mcrypt_ecb(cipher->value.lval, key->value.str.val, key->value.str.len);
	if(td == -1) {
		php3_error(E_WARNING, "mcrypt initialization failed");
		RETURN_FALSE;
	}
	
	ndata = ecalloc(nr, bsize);
	memcpy(ndata, data->value.str.val, data->value.str.len);
	
	if(mode->value.lval == 0)
		mcrypt_ecb(td, ndata, nsize);
	else
		mdecrypt_ecb(td, ndata, nsize);
	
	end_mcrypt(td);

	RETURN_STRINGL(ndata, nsize, 0);
}

#endif
