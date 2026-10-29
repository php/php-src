--TEST--
Request boundary starts the session of the next request when session.auto_start is enabled
--EXTENSIONS--
session
zend_test
--INI--
session.auto_start=1
session.use_cookies=0
session.cache_limiter=
session.save_handler=files
--FILE--
<?php
$previous = session_id();
session_write_close();

var_dump(zend_test_request_boundary());

var_dump(session_status() === PHP_SESSION_ACTIVE, session_id() !== $previous);
session_destroy();
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
