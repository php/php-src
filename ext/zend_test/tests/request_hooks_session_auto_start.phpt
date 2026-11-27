--TEST--
Request hooks don't retry a failed session.auto_start in the request RINIT already tried it for
--EXTENSIONS--
session
zend_test
--INI--
session.auto_start=1
session.save_handler=files
session.save_path=/nonexistent/request_hooks
session.use_cookies=0
session.cache_limiter=
--FILE--
<?php
var_dump(session_status() === PHP_SESSION_NONE);
?>
--EXPECTF--
Warning: PHP Request Startup: open(%s) failed: %s in Unknown on line 0

Warning: PHP Request Startup: Failed to read session data: files (path: %s) in Unknown on line 0
bool(true)
