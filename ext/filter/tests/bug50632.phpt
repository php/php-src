--TEST--
bug 50632, filter_input() does not return default value if the variable does not exist
--EXTENSIONS--
filter
--FILE--
<?php
$foo = filter_input(INPUT_GET, 'foo', FILTER_VALIDATE_INT, array('flags' => FILTER_REQUIRE_SCALAR, 'options' => array('default' => 23)));
var_dump($foo);
?>
--EXPECT--
int(23)
