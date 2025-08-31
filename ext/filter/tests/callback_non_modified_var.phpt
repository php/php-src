--TEST--
callback function returns non modified value
--EXTENSIONS--
filter
--FILE--
<?php
function callback($var) {
    return $var;
}
$var = "test";
var_dump(filter_var($var, FILTER_CALLBACK, array('options'=>'callback')));
?>
--EXPECT--
string(4) "test"
