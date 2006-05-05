--TEST--
filter_data() and FILTER_VALIDATE_BOOLEAN
--FILE--
<?php

var_dump(filter_data("no", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data(new stdClass, FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("yes", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("true", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("false", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("off", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("on", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("0", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("1", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("NONE", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data(-1, FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("000000", FILTER_VALIDATE_BOOLEAN));
var_dump(filter_data("111111", FILTER_VALIDATE_BOOLEAN));
	

echo "Done\n";
?>
--EXPECTF--	
bool(false)

Notice: Object of class stdClass to string conversion in %s on line %d
NULL
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
NULL
bool(false)
NULL
NULL
NULL
Done
