--TEST--
Bug #53248 (rawurlencode RFC 3986 EBCDIC support)
--FILE--
<?php

var_dump(rawurlencode('A1_-.~'));
var_dump(rawurldecode('%41%31%5F%2D%2E%7E'));

?>
--EXPECT--
string(6) "A1_-.~"
string(6) "A1_-.~"
