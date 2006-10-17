--TEST--
filter_id()
--FILE--
<?php

var_dump(filter_id("stripped"));
var_dump(filter_id("string"));
var_dump(filter_id("url"));
var_dump(filter_id("int"));
var_dump(filter_id("none"));
var_dump(filter_id(array()));
var_dump(filter_id(-1));
var_dump(filter_id(0,0,0));

echo "Done\n";
?>
--EXPECTF--	
int(513)
int(513)
int(518)
int(257)
<<<<<<< 009.phpt
bool(false)

Warning: filter_id() expects parameter 1 to be string, array given in %s on line %d
NULL
bool(false)
=======
bool(false)
>>>>>>> 1.4

<<<<<<< 009.phpt
Warning: filter_id() expects exactly 1 parameter, 3 given in %s on line %d
NULL
Done
--UEXPECTF--
int(513)
int(513)
int(518)
int(257)
NULL

Warning: input_name_to_filter() expects parameter 1 to be binary string, array given in %s on line %d
NULL
=======
Warning: filter_id() expects parameter 1 to be string, array given in %s on line %d
>>>>>>> 1.4
NULL
bool(false)

Warning: filter_id() expects exactly 1 parameter, 3 given in %s on line %d
NULL
Done
