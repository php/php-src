--TEST--
filter_data() and FL_BOOLEAN
--GET--

--FILE--
<?php

var_dump(filter_data("no", FL_BOOLEAN));
var_dump(filter_data(new stdClass, FL_BOOLEAN));
var_dump(filter_data("yes", FL_BOOLEAN));
var_dump(filter_data("true", FL_BOOLEAN));
var_dump(filter_data("false", FL_BOOLEAN));
var_dump(filter_data("off", FL_BOOLEAN));
var_dump(filter_data("on", FL_BOOLEAN));
var_dump(filter_data("0", FL_BOOLEAN));
var_dump(filter_data("1", FL_BOOLEAN));
var_dump(filter_data("NONE", FL_BOOLEAN));
var_dump(filter_data("", FL_BOOLEAN));
var_dump(filter_data(-1, FL_BOOLEAN));
var_dump(filter_data("000000", FL_BOOLEAN));
var_dump(filter_data("111111", FL_BOOLEAN));
	

echo "Done\n";
?>
--EXPECTF--	
bool(false)
PHP Notice:  Object of class stdClass to string conversion in %s on line %d

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
