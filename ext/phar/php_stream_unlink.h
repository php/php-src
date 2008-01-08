#ifndef PHAR_UNLINK
#define PHAR_UNLINK
#if defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 50400 || PHP_VERSION_ID >= 60000)

static int _php_stream_unlink(char *url, int options, php_stream_context *context TSRMLS_DC)
{
	php_stream_wrapper *wrapper = php_stream_locate_url_wrapper(url, NULL, options TSRMLS_CC);

	if (!wrapper || !wrapper->wops) {
		return 0;
	}

	if (!wrapper->wops->unlink) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC, "%s does not allow unlinking", wrapper->wops->label ? wrapper->wops->label : "Wrapper");
		return 0;
	}
	return wrapper->wops->unlink(wrapper, url, ENFORCE_SAFE_MODE | REPORT_ERRORS, context TSRMLS_CC);
}

#define php_stream_unlink(url, options, context) _php_stream_unlink((url), (options), (context) TSRMLS_CC)

#endif /* defined(PHP_VERSION_ID) && (PHP_VERSION_ID < 50400 || PHP_VERSION_ID >= 60000) */
#endif /* PHAR_UNLINK */
