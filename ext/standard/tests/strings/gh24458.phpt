--TEST--
GH-24458 (quoted_printable_encode does not encode trailing space)
--FILE--
<?php
var_dump(quoted_printable_encode('hello '));
var_dump(quoted_printable_encode('hello  '));
var_dump(quoted_printable_encode(' '));
var_dump(quoted_printable_decode(quoted_printable_encode('hello ')));
?>
--EXPECT--
string(8) "hello=20"
string(9) "hello =20"
string(3) "=20"
string(6) "hello "
