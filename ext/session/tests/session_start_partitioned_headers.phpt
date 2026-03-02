--TEST--
session_start() with partitioned cookies - header test
--EXTENSIONS--
session
--FILE--
<?php
session_id('12345');
session_set_cookie_params(["secure" => true, "partitioned" => true]);
session_start();
?>
--EXPECTHEADERS--
Set-Cookie: PHPSESSID=12345; path=/; secure; Partitioned
--EXPECT--
