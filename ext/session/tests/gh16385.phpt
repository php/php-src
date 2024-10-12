--TEST--
GH-16385 (Unexpected null returned by session_set_cookie_params)
--EXTENSIONS--
session
--INI--
session.use_cookies=0
--FILE--
<?php
var_dump(session_set_cookie_params(3600, "/foo"));
?>
--EXPECTF--
Warning: session_set_cookie_params(): Session cookies cannot be used when session.use_cookies is disabled in %s on line %d
bool(false)
