--TEST--
GH-21998 (NumberFormatter::format(INF) leaves a non-NUL-terminated zend_string)
--EXTENSIONS--
intl
--FILE--
<?php
$fmt = new NumberFormatter("en", NumberFormatter::DECIMAL);
var_dump($fmt->format(INF));
var_dump($fmt->format(-INF));
var_dump($fmt->format(NAN));
?>
--EXPECT--
string(3) "∞"
string(4) "-∞"
string(3) "NaN"
