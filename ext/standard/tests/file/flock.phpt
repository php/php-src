--TEST--
flock() tests
--FILE--
<?php

$file = dirname(__FILE__)."/flock.dat";

var_dump(flock());
var_dump(flock("", "", $var));

$fp = fopen($file, "w");
fclose($fp);

var_dump(flock($fp, LOCK_SH|LOCK_NB));

$fp = fopen($file, "w");

var_dump(flock($fp, LOCK_SH|LOCK_NB));
var_dump(flock($fp, LOCK_UN));
var_dump(flock($fp, LOCK_EX));
var_dump(flock($fp, LOCK_UN));

$would = array(1,2,3);
var_dump(flock($fp, LOCK_SH|LOCK_NB, $would));
var_dump($would);
var_dump(flock($fp, LOCK_UN, $would));
var_dump($would);
var_dump(flock($fp, LOCK_EX, $would));
var_dump($would);
var_dump(flock($fp, LOCK_UN, $would));
var_dump($would);

var_dump(flock($fp, -1));
var_dump(flock($fp, 0));

echo "Done\n";
?>
--CLEAN--
<?php
$file = dirname(__FILE__)."/flock.dat";
unlink($file);
?>
--EXPECTF--
Warning: flock() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: flock() expects parameter 1 to be resource, string given in %s on line %d
NULL

Warning: flock(): supplied resource is not a valid stream resource in %s on line %d
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
int(0)
bool(true)
int(0)
bool(true)
int(0)
bool(true)

Warning: flock(): Illegal operation argument in %s on line %d
bool(false)
Done
