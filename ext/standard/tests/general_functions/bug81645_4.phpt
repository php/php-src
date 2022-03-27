--TEST--
Bug #81645 (header() allows arbitrary status codes) integer overflow
--FILE--
<?php
header("HTTP/1.1 4294967496 There was overflow");
?>
--EXPECTF--
Warning: Cannot set HTTP status line - status code is malformed in %s on line %d
