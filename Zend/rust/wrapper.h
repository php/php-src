/* Wrapper header for bindgen
 * This provides minimal definitions needed for Rust bindings
 * without pulling in the entire PHP header tree
 */

#include <stdlib.h>
#include <string.h>

/* Stub definitions for PHP functions we might want to call from Rust
 * In a real integration, these would come from the actual PHP headers
 */

/* These would normally come from zend_errors.h */
void zend_error_noreturn(int type, const char *format, ...);

/* These would normally come from zend_alloc.h */
void zend_out_of_memory(void);