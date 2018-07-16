--TEST--
__compareTo: Comparison always returns -1, 0, or 1
--FILE--
<?php

var_dump(new class { public function __compareTo($other) { return 2;            } } <=> 1);
var_dump(new class { public function __compareTo($other) { return -2;           } } <=> 1);
var_dump(new class { public function __compareTo($other) { return 0.5;          } } <=> 1); // Not truncated
var_dump(new class { public function __compareTo($other) { return -0.5;         } } <=> 1); // Not truncated
var_dump(new class { public function __compareTo($other) { return 'a';          } } <=> 1);
var_dump(new class { public function __compareTo($other) { return true;         } } <=> 1);
var_dump(new class { public function __compareTo($other) { return new stdClass; } } <=> 1); // Attempt to convert
var_dump(new class { public function __compareTo($other) {                      } } <=> 1);

?>
--EXPECTF--
int(1)
int(-1)
int(1)
int(-1)
int(0)
int(1)

Notice: Object of class stdClass could not be converted to int in %s on line %d
int(1)
int(0)
