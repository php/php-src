--TEST--
session_start() with partitioned cookies - header test
--EXTENSIONS--
session
--INI--
session.use_strict_mode=0
session.cookie_samesite=Lax
session.cookie_httponly=1
--FILE--
<?php
session_id('12345');
session_set_cookie_params(["secure" => true, "partitioned" => true]);
session_start();
?>
--EXPECTHEADERS--
Set-Cookie: PHPSESSID=12345; path=/; secure; Partitioned; HttpOnly; SameSite=Lax
--EXPECT--
