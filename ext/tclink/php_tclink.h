/*
 * TCLink PHP Module
 *
 * TCLink Copyright (c) 2002 TrustCommerce.
 * http://www.trustcommerce.com
 * developer@trustcommerce.com
 * (626) 744-7700
 */

#ifndef PHP_TCLINK_H
#define PHP_TCLINK_H

/* Handle passed to all TCLink functions.  A unique handle must be created
 * for each concurrent thread, but the same handle can be shared by transactions
 * occurring one after another (such as a for loop).
 */
#define TCLinkHandle void *

/* Parameter names and values cannot exceed this size.
 */
#define PARAM_MAX_LEN 256

/* Create a new TCLinkHandle.
 */
TCLinkHandle TCLinkCreate();

/* Add a parameter to be sent to the server.
 */
void TCLinkPushParam(TCLinkHandle handle, const char *name, const char *value);

/* Flush the parameters to the server.
 */
void TCLinkSend(TCLinkHandle handle);

/* Look up a response value from the server.
 * Returns NULL if no such parameter, or stores the value in 'value' and
 * returns a pointer to value.  value should be at least PARAM_MAX_LEN in size.
 */
char *TCLinkGetResponse(TCLinkHandle handle, const char *name, char *value);

/* Get all response values from the server in one giant string.
 * Stores the string into buf and returns a pointer to it.  Size should be
 * sizeof(buf), which will limit the string so that no buffer overruns occur.
 */
char *TCLinkGetEntireResponse(TCLinkHandle handle, char *buf, int size);

/* Destory a handle, ending that transaction and freeing the memory associated with it. */
void TCLinkDestroy(TCLinkHandle handle);

/* Store version string into buf.  Returns a pointer to buf. */
char *TCLinkGetVersion(char *buf);

#if HAVE_TCLINK
extern zend_module_entry php_tclink_module_entry;
#define tclink_module_ptr &php_tclink_module_entry

PHP_MINFO_FUNCTION(tclink);

PHP_FUNCTION(tclink_send);
PHP_FUNCTION(tclink_getversion);

#else
#define tclink_module_ptr NULL
#endif

#define phpext_tclink_ptr tclink_module_ptr

#endif /* PHP_TCLINK_H */
