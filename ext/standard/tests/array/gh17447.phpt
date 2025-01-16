--TEST--
GH-17447 (Assertion failure when array poping a self addressing variable)
--FILE--
<?php
$input[] = &$input;
var_dump(array_pop($input), $input);
?>
--EXPECT--
array(0) {
}
array(0) {
}
