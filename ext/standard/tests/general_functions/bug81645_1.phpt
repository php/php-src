--TEST--
Bug #81645 (header() allows arbitrary status codes) leading zero
--FILE--
<?php
header("HTTP/1.1 000 ZERO");
?>
--EXPECTF--
Warning: Cannot set HTTP status line - status code is malformed in %s on line %d
