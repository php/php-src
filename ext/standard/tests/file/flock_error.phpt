--TEST--
Test flock() function: Error conditions
--FILE--
<?php
/* 
Prototype: bool flock(resource $handle, int $operation [, int &$wouldblock]);
Description: PHP supports a portable way of locking complete files 
  in an advisory way
*/

echo "*** Testing error conditions ***\n";

$file = dirname(__FILE__)."/flock.tmp";
$fp = fopen($file, "w");

/* array of operatons */
$operations = array(
  0,
  LOCK_NB,
  FALSE,
  NULL,
  array(1,2,3),
  array(),
  "string",
  "",
  "\0" 
);

$i = 0;
foreach($operations as $operation) {
  echo "\n--- Iteration $i ---";
  var_dump(flock($fp, $operation));
  $i++;
}


/* Invalid arguments */
$fp = fopen($file, "w");
fclose($fp);
var_dump(flock($fp, LOCK_SH|LOCK_NB));

var_dump(flock("", "", $var));

/* No.of args leass than expected */
var_dump(flock());
var_dump(flock($fp));

/* No.of args greater than expected */
var_dump(flock($fp, "", $var, ""));

echo "\n*** Done ***\n";
?>
--CLEAN--
<?php
$file = dirname(__FILE__)."/flock.tmp";
unlink($file);
?>
--EXPECTF--	
*** Testing error conditions ***

--- Iteration 0 ---
Warning: flock(): Illegal operation argument in %s on line %d
bool(false)

--- Iteration 1 ---
Warning: flock(): Illegal operation argument in %s on line %d
bool(false)

--- Iteration 2 ---
Warning: flock(): Illegal operation argument in %s on line %d
bool(false)

--- Iteration 3 ---
Warning: flock(): Illegal operation argument in %s on line %d
bool(false)

--- Iteration 4 ---
Warning: flock() expects parameter 2 to be long, array given in %s on line %d
NULL

--- Iteration 5 ---
Warning: flock() expects parameter 2 to be long, array given in %s on line %d
NULL

--- Iteration 6 ---
Warning: flock() expects parameter 2 to be long, string given in %s on line %d
NULL

--- Iteration 7 ---
Warning: flock() expects parameter 2 to be long, string given in %s on line %d
NULL

--- Iteration 8 ---
Warning: flock() expects parameter 2 to be long, string given in %s on line %d
NULL

Warning: flock(): 6 is not a valid stream resource in %s on line %d
bool(false)

Warning: flock() expects parameter 1 to be resource, string given in %s on line %d
NULL

Warning: flock() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: flock() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: flock() expects at most 3 parameters, 4 given in %s on line %d
NULL

*** Done ***
