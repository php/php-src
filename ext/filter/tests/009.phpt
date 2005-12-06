--TEST--
input_name_to_filter()
--GET--

--FILE--
<?php

var_dump(input_name_to_filter("stripped"));
var_dump(input_name_to_filter("string"));
var_dump(input_name_to_filter("url"));
var_dump(input_name_to_filter("int"));
var_dump(input_name_to_filter("none"));
var_dump(input_name_to_filter(array()));
var_dump(input_name_to_filter(-1));
var_dump(input_name_to_filter(0,0,0));

echo "Done\n";
?>
--EXPECTF--	
int(513)
int(513)
int(518)
int(257)
NULL
PHP Warning:  input_name_to_filter() expects parameter 1 to be string, array given in %s on line %d

Warning: input_name_to_filter() expects parameter 1 to be string, array given in %s on line %d
NULL
NULL
PHP Warning:  input_name_to_filter() expects exactly 1 parameter, 3 given in %s on line %d

Warning: input_name_to_filter() expects exactly 1 parameter, 3 given in %s on line %d
NULL
Done
