--TEST--
__compareTo: Check that comparing to NULL is not broken for existing objects
--FILE--
<?php

/**
 * Check that throwing an exception in the comparison function works.
 */
var_dump(new stdClass ==  null);
var_dump(new stdClass >=  null);
var_dump(new stdClass <=  null);
var_dump(new stdClass <   null);
var_dump(new stdClass >   null);
var_dump(new stdClass <=> null);

echo "\n";

var_dump(null ==  new stdClass);
var_dump(null >=  new stdClass);
var_dump(null <=  new stdClass);
var_dump(null <   new stdClass);
var_dump(null >   new stdClass);
var_dump(null <=> new stdClass);

?>
--EXPECTF--
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
int(1)

bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
int(-1)
