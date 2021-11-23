--TEST--
Bug #81645 (header() allows arbitrary status codes) leading zero
--FILE--
<?php
header("HTTP/1.1 000 ZERO");
?>
--EXPECTHEADERS--
Status: 000 ZERO
--EXPECT--
