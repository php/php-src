--TEST--
filter_var() and default values
--EXTENSIONS--
filter
--FILE--
<?php
$tmp = $default = 321;
var_dump(filter_var("123asd", FILTER_VALIDATE_INT, array("options"=>array("default"=>$default))));
?>
--EXPECT--
int(321)
