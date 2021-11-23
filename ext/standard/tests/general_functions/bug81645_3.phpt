--TEST--
Bug #81645 (header() allows arbitrary status codes) longer than 3 digit
--FILE--
<?php
header("HTTP/1.1 1234 One Two Three Four");
?>
--EXPECTF--
Warning: Cannot set HTTP status line - status code is malformed in %s on line %d
