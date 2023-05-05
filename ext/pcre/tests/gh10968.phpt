--TEST--
GH-10968: preg_replace_callback_array() segmentation fault
--FILE--
<?php
var_dump(preg_replace_callback_array([], []));
var_dump(preg_replace_callback_array([], ''));
?>
--EXPECT--
array(0) {
}
string(0) ""
