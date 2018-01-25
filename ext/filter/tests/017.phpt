--TEST--
filter_var() and FILTER_VALIDATE_REGEXP
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

var_dump(filter_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/.*/'))));
var_dump(filter_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/^b(.*)/'))));
var_dump(filter_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/^d(.*)/'))));
var_dump(filter_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/blah/'))));
var_dump(filter_var("data", FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>'/\[/'))));
var_dump(filter_var("data", FILTER_VALIDATE_REGEXP));

echo "Done\n";
?>
--EXPECTF--
string(4) "data"
bool(false)
string(4) "data"
bool(false)
bool(false)

Warning: filter_var(): 'regexp' option missing in %s on line %d
bool(false)
Done
