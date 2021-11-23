--TEST--
Bug #81645 (header() allows arbitrary status codes) 2 digit code
--FILE--
<?php
header("HTTP/1.1 11 ELEVEN");
?>
--EXPECTF--
Warning: Cannot set HTTP status line - status code is malformed in %s on line %d
