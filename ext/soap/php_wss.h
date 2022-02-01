#ifndef PHP_WSS_H
#define PHP_WSS_H 

#define SOAP_WSS_DIGEST_METHOD_SHA1		1
#define SOAP_WSS_DIGEST_METHOD_SHA256	2
#define SOAP_WSS_DIGEST_METHOD_SHA512	3

#define SOAP_WSS_VERSION_1_0	1
#define SOAP_WSS_VERSION_1_1	2

#define SOAP_WSS_TIMESTAMP_EXPIRES	300

#define SOAP_WSS_BASE64_BINARY			"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary"
#define SOAP_WSS_EXC_C14N				"http://www.w3.org/2001/10/xml-exc-c14n#"
#define SOAP_WSS_RSA_SHA1				"http://www.w3.org/2000/09/xmldsig#rsa-sha1"
#define SOAP_WSS_SHA1					"http://www.w3.org/2000/09/xmldsig#sha1"
#define SOAP_WSS_SHA256					"http://www.w3.org/2001/04/xmlenc#sha256"
#define SOAP_WSS_SHA512					"http://www.w3.org/2001/04/xmlenc#sha512"
#define SOAP_WSS_SECEXT_1_0				"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"
#define SOAP_WSS_SECEXT_1_1				"http://docs.oasis-open.org/wss/oasis-wss-wssecurity-secext-1.1.xsd"
#define SOAP_WSS_UTILITY				"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd"
#define SOAP_WSS_X509v3_TOKEN_PROFILE	"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-x509-token-profile-1.0#X509v3"
#define SOAP_WSS_XMLDSIG				"http://www.w3.org/2000/09/xmldsig#"

void add_wss_to_function_call(zval *this_ptr, int soap_version, xmlNodePtr envelope, xmlNodePtr head, xmlNodePtr body);

#endif
