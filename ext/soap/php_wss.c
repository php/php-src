#include "php_soap.h"
#include "ext/date/php_date.h"
#include "ext/hash/php_hash.h"
#include "ext/standard/base64.h"
#include "ext/standard/php_mt_rand.h"
#include <libxml/c14n.h>
#include <libxml/xpathInternals.h>

static zend_string *get_exc_c14n_from_node_ptr(xmlNodePtr node) /* {{{ */
{
	zend_string *result = NULL;
	
	xmlOutputBufferPtr buffer = xmlAllocOutputBuffer(NULL);
	if (buffer != NULL) {
		xmlXPathContextPtr xpath_ctx = xmlXPathNewContext(node->doc);
		if(xpath_ctx != NULL) {
			xpath_ctx->node = node;
			xmlXPathObjectPtr xpath_obj = xmlXPathEvalExpression(BAD_CAST("(.//. | .//@* | .//namespace::*)"), xpath_ctx);
			xpath_ctx->node = NULL;
			
			if (xpath_obj != NULL && xpath_obj->type == XPATH_NODESET) {
				int c14n_ret = xmlC14NDocSaveTo(node->doc, xpath_obj->nodesetval, XML_C14N_EXCLUSIVE_1_0, NULL, 0, buffer);
				if (c14n_ret >= 0) {
					#ifdef LIBXML2_NEW_BUFFER
						int buffer_size = xmlOutputBufferGetSize(buffer);
					#else
						int buffer_size = buffer->buffer->use;
					#endif
					if (buffer_size > 0) {
						#ifdef LIBXML2_NEW_BUFFER
							char *buffer_content = (char *)xmlOutputBufferGetContent(buffer);
						#else
							char *buffer_content = (char *)buffer->buffer->content;
						#endif
						result = zend_string_init(buffer_content, strlen(buffer_content), 0);
					}
				}
			}

			xmlXPathFreeObject(xpath_obj);
			xmlXPathFreeContext(xpath_ctx);
		}
		xmlOutputBufferClose(buffer);
	}

	return result;
}
/* }}} */

static int add_reference_to_signed_info(zval *this_ptr, xmlNodePtr signed_info, xmlNodePtr source, zend_string *source_uri, xmlNsPtr ns_ds) /* {{{ */
{
	xmlNodePtr reference = xmlNewChild(signed_info, ns_ds, BAD_CAST("Reference"), NULL);
	xmlSetProp(reference, BAD_CAST("URI"), BAD_CAST(ZSTR_VAL(source_uri)));
	xmlNodePtr reference_trsfrms = xmlNewChild(reference, ns_ds, BAD_CAST("Transforms"), NULL);
	xmlNodePtr reference_trsfrm = xmlNewChild(reference_trsfrms, ns_ds, BAD_CAST("Transform"), NULL);
	xmlSetProp(reference_trsfrm, BAD_CAST("Algorithm"), BAD_CAST(SOAP_WSS_EXC_C14N));
	xmlNodePtr reference_dm = xmlNewChild(reference, ns_ds, BAD_CAST("DigestMethod"), NULL);
	
	const php_hash_ops *ops = NULL;
	zend_string *algo = NULL;
	zend_long digest_method = Z_LVAL_P(Z_CLIENT_WSS_DIGEST_METHOD_P(this_ptr));
	switch (digest_method) {
		case SOAP_WSS_DIGEST_METHOD_SHA1:			
			algo = zend_string_init("sha1", strlen("sha1"), 0);
			ops = php_hash_fetch_ops(algo);
			xmlSetProp(reference_dm, BAD_CAST("Algorithm"), BAD_CAST(SOAP_WSS_SHA1));
			break;
		case SOAP_WSS_DIGEST_METHOD_SHA256:
			algo = zend_string_init("sha256", strlen("sha256"), 0);
			ops = php_hash_fetch_ops(algo);
			xmlSetProp(reference_dm, BAD_CAST("Algorithm"), BAD_CAST(SOAP_WSS_SHA256));
			break;
		case SOAP_WSS_DIGEST_METHOD_SHA512:
			algo = zend_string_init("sha512", strlen("sha512"), 0);
			ops = php_hash_fetch_ops(algo);
			xmlSetProp(reference_dm, BAD_CAST("Algorithm"), BAD_CAST(SOAP_WSS_SHA512));			
			break;
	}

	if (ops == NULL) {
		if (algo != NULL) {
			php_error_docref(NULL, E_WARNING, "invalid hashing algorithm, %s", ZSTR_VAL(algo));
			zend_string_release_ex(algo, 0);
		} else {
			php_error_docref(NULL, E_WARNING, "unknown hashing algorithm");
		}
		return 0;
	}

	zend_string_release_ex(algo, 0);

	zend_string *exc_c14n = get_exc_c14n_from_node_ptr(source);
	if (exc_c14n != NULL) {
		void *context = php_hash_alloc_context(ops);
		ops->hash_init(context, NULL);
		ops->hash_update(context, (unsigned char *)ZSTR_VAL(exc_c14n), ZSTR_LEN(exc_c14n));
		
		zend_string *digest = zend_string_alloc(ops->digest_size, 0);
		ops->hash_final((unsigned char *)ZSTR_VAL(digest), context);
		efree(context);

		ZSTR_VAL(digest)[ops->digest_size] = 0;

		zend_string *digest_base64 = php_base64_encode((unsigned char *)ZSTR_VAL(digest), ZSTR_LEN(digest));
		xmlNewChild(reference, ns_ds, BAD_CAST("DigestValue"), BAD_CAST(ZSTR_VAL(digest_base64)));

		zend_string_release(digest_base64);
		zend_string_release(digest);
		zend_string_release_ex(exc_c14n, 0);

		return 1;
	} else {
		php_error_docref(NULL, E_WARNING, "exclusive C14N failed, URI \"%s\"", ZSTR_VAL(source_uri));
		return 0;
	}
}
/* }}} */

void add_wss_to_function_call(zval *this_ptr, int soap_version, xmlNodePtr envelope, xmlNodePtr head, xmlNodePtr body) /* {{{ */
{
	char random_s[10];
	if (Z_TYPE_P(Z_CLIENT_WSS_RANDOM_ID_P(this_ptr)) == IS_TRUE) {
		slprintf(random_s, sizeof(random_s), "%d", (int)php_mt_rand_common(100000000,999999999));
	} else {
		slprintf(random_s, sizeof(random_s), "%d", 1);
	}

	zend_string *body_id = zend_string_concat2(
		"BodyID-", strlen("BodyID-"),
		random_s, strlen(random_s));
	zend_string *body_id_uri = zend_string_concat2(
		"#", strlen("#"),
		ZSTR_VAL(body_id), ZSTR_LEN(body_id));
	
	xmlNsPtr ns_wsse = NULL;
	if (Z_LVAL_P(Z_CLIENT_WSS_VERSION_P(this_ptr)) == SOAP_WSS_VERSION_1_1) {
		ns_wsse = xmlNewNs(envelope, BAD_CAST(SOAP_WSS_SECEXT_1_1), BAD_CAST("wsse11"));
	} else {
		ns_wsse = xmlNewNs(envelope, BAD_CAST(SOAP_WSS_SECEXT_1_0), BAD_CAST("wsse"));
	}

	xmlNsPtr ns_ds = xmlNewNs(envelope, BAD_CAST(SOAP_WSS_XMLDSIG), BAD_CAST("ds"));
	xmlNsPtr ns_wsu = xmlNewNs(envelope, BAD_CAST(SOAP_WSS_UTILITY), BAD_CAST("wsu"));
	xmlNodePtr security = xmlNewChild(head, ns_wsse, BAD_CAST("Security"), NULL);
	xmlSetNsProp(body, ns_wsu, BAD_CAST("Id"), BAD_CAST(ZSTR_VAL(body_id)));

	if (soap_version == SOAP_1_1) {
		xmlSetProp(security, BAD_CAST(SOAP_1_1_ENV_NS_PREFIX":mustUnderstand"), BAD_CAST("1"));
	} else {
		xmlSetProp(security, BAD_CAST(SOAP_1_2_ENV_NS_PREFIX":mustUnderstand"), BAD_CAST("true"));
	}
	
	xmlNodePtr timestamp = NULL;
	zend_string *timestamp_id = NULL, *timestamp_id_uri = NULL;
	if (Z_TYPE_P(Z_CLIENT_WSS_ADD_TIMESTAMP_P(this_ptr)) == IS_TRUE) {
		char *time_format = "Y-m-d\\TH:i:s\\Z";
		zend_long now = (zend_long)php_time();
		zend_long expires = Z_LVAL_P(Z_CLIENT_WSS_TIMESTAMP_EXPIRES_P(this_ptr));
		zend_string *timestamp_s = php_format_date(time_format, strlen(time_format), now, 0);
		zend_string *expires_s = php_format_date(time_format, strlen(time_format), (now + expires), 0);
		
		timestamp_id = zend_string_concat2(
			"TimestampID-", strlen("TimestampID-"),
			random_s, strlen(random_s));
		timestamp_id_uri = zend_string_concat2(
			"#", strlen("#"),
			ZSTR_VAL(timestamp_id), ZSTR_LEN(timestamp_id));
		
		timestamp = xmlNewChild(security, ns_wsu, BAD_CAST("Timestamp"), NULL);
		xmlSetNsProp(timestamp, ns_wsu, BAD_CAST("Id"), BAD_CAST(ZSTR_VAL(timestamp_id)));
		xmlNewChild(timestamp, ns_wsu, BAD_CAST("Created"), BAD_CAST(ZSTR_VAL(timestamp_s)));
		xmlNewChild(timestamp, ns_wsu, BAD_CAST("Expires"), BAD_CAST(ZSTR_VAL(expires_s)));
		
		zend_string_release(expires_s);
		zend_string_release(timestamp_s);
	}

	int references = 0;
	xmlNodePtr signed_info, signature_value = NULL;
	if (Z_TYPE_P(Z_CLIENT_WSS_SIGNFUNC_P(this_ptr)) != IS_NULL) {
		zend_string *token_id = NULL, *token_id_uri = NULL;
		if (Z_TYPE_P(Z_CLIENT_WSS_X509_BINSECTOKEN_P(this_ptr)) == IS_STRING) {
			token_id = zend_string_concat2(
				"TokenID-", strlen("TokenID-"),
				random_s, strlen(random_s));
			token_id_uri = zend_string_concat2(
				"#", strlen("#"),
				ZSTR_VAL(token_id), ZSTR_LEN(token_id));
			
			zend_string *token_base64 = php_base64_encode((unsigned char *)Z_STRVAL_P(Z_CLIENT_WSS_X509_BINSECTOKEN_P(this_ptr)), Z_STRLEN_P(Z_CLIENT_WSS_X509_BINSECTOKEN_P(this_ptr)));

			xmlNodePtr x509_binsectoken = xmlNewChild(security, ns_wsse, BAD_CAST("BinarySecurityToken"), BAD_CAST(ZSTR_VAL(token_base64)));
			xmlSetProp(x509_binsectoken, BAD_CAST("EncodingType"), BAD_CAST(SOAP_WSS_BASE64_BINARY));
			xmlSetProp(x509_binsectoken, BAD_CAST("ValueType"), BAD_CAST(SOAP_WSS_X509v3_TOKEN_PROFILE));
			xmlSetNsProp(x509_binsectoken, ns_wsu, BAD_CAST("Id"), BAD_CAST(ZSTR_VAL(token_id)));

			zend_string_release(token_base64);
		}

		xmlNodePtr signature = xmlNewChild(security, ns_ds, BAD_CAST("Signature"), NULL);
		signed_info = xmlNewChild(signature, ns_ds, BAD_CAST("SignedInfo"), NULL);
		xmlNodePtr canon_method = xmlNewChild(signed_info, ns_ds, BAD_CAST("CanonicalizationMethod"), NULL);
		xmlNodePtr sign_method = xmlNewChild(signed_info, ns_ds, BAD_CAST("SignatureMethod"), NULL);
		xmlSetProp(canon_method, BAD_CAST("Algorithm"), BAD_CAST(SOAP_WSS_EXC_C14N));
		xmlSetProp(sign_method, BAD_CAST("Algorithm"), BAD_CAST(SOAP_WSS_RSA_SHA1));

		if (timestamp != NULL) {
			references += add_reference_to_signed_info(this_ptr, signed_info, timestamp, timestamp_id_uri, ns_ds);
		}
		references += add_reference_to_signed_info(this_ptr, signed_info, body, body_id_uri, ns_ds);

		signature_value = xmlNewChild(signature, ns_ds, BAD_CAST("SignatureValue"), NULL);

		if (token_id != NULL) {
			xmlNodePtr keyinfo = xmlNewChild(signature, ns_ds, BAD_CAST("KeyInfo"), NULL);
			xmlNodePtr keyinfo_sectoken_reference = xmlNewChild(keyinfo, ns_wsse, BAD_CAST("SecurityTokenReference"), NULL);
			xmlNodePtr keyinfo_sectoken_reference_ref = xmlNewChild(keyinfo_sectoken_reference, ns_wsse, BAD_CAST("Reference"), NULL);
			xmlSetProp(keyinfo_sectoken_reference_ref, BAD_CAST("URI"), BAD_CAST(ZSTR_VAL(token_id_uri)));
			xmlSetProp(keyinfo_sectoken_reference_ref, BAD_CAST("ValueType"), BAD_CAST(SOAP_WSS_X509v3_TOKEN_PROFILE));
			zend_string_release_ex(token_id_uri, 0);
			zend_string_release_ex(token_id, 0);
		}
	}

	if (timestamp_id != NULL) {
		zend_string_release_ex(timestamp_id_uri, 0);
		zend_string_release_ex(timestamp_id, 0);
	}

	bool bailout = 0;
	if (references != 0) {
		zend_string *signed_info_exc_c14n = get_exc_c14n_from_node_ptr(signed_info);
		if (signed_info_exc_c14n != NULL) {
			zend_fcall_info signature_fci;
			zend_fcall_info_cache signature_fcc;
			if (zend_fcall_info_init(Z_CLIENT_WSS_SIGNFUNC_P(this_ptr), 0, &signature_fci, &signature_fcc, NULL, NULL) == SUCCESS) {
				zval signature_params, signature_retval;
				
				ZVAL_STR(&signature_params, signed_info_exc_c14n);
				
				signature_fci.param_count = 1;
				signature_fci.params = &signature_params;
				signature_fci.retval = &signature_retval;
				
				if (zend_call_function(&signature_fci, &signature_fcc) == SUCCESS) {
					if (Z_TYPE(signature_retval) == IS_STRING) {
						zend_string *signature_base64 = php_base64_encode((unsigned char *)Z_STRVAL(signature_retval), Z_STRLEN(signature_retval));
						xmlNodeSetContent(signature_value, BAD_CAST(ZSTR_VAL(signature_base64)));
						zend_string_release(signature_base64);
					} else {
						if (!EG(exception)) {
							php_error_docref(NULL, E_WARNING, "\"signfunc\" should return a string, %s given", zend_zval_type_name(&signature_retval));
						} else {
							bailout = 1;
						}
					}
				} else {
					php_error_docref(NULL, E_WARNING, "unable to run \"signfunc\"");
				}

				zval_ptr_dtor_str(&signature_params);
				zval_ptr_dtor(&signature_retval);
			} else {
				php_error_docref(NULL, E_WARNING, "unable to initialize \"signfunc\"");
				zend_string_release_ex(signed_info_exc_c14n, 0);
			}
		} else {
			 php_error_docref(NULL, E_WARNING, "exclusive C14N failed, element \"SignedInfo\"");
		}
	}
	
	zend_string_release_ex(body_id_uri, 0);
	zend_string_release_ex(body_id, 0);

	if (bailout) {
		zend_bailout();
	}
}
/* }}} */
