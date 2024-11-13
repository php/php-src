--TEST--
GH-15087 (IntlChar::foldCase()'s $option is not optional)
--EXTENSIONS--
intl
--FILE--
<?php
var_dump(IntlChar::foldCase('I'));
?>
--EXPECT--
string(1) "i"
