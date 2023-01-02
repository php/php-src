--TEST--
Testing operations with undefined variable
--FILE--
<?php

var_dump($a[1]);
var_dump($a[$c]);
var_dump($a + 1);
var_dump($a + $b);
var_dump($a++);
var_dump(++$b);
var_dump($a->$b);
var_dump($a->$b);
var_dump($a->$b->{$c[1]});

?>
--EXPECTF--
Warning: Undefined variable $a (this will become an error in PHP 9.0) in %s on line %d

Warning: Trying to access array offset on value of type null in %s on line %d
NULL

Warning: Undefined variable $a (this will become an error in PHP 9.0) in %s on line %d

Warning: Undefined variable $c (this will become an error in PHP 9.0) in %s on line %d

Warning: Trying to access array offset on value of type null in %s on line %d
NULL

Warning: Undefined variable $a (this will become an error in PHP 9.0) in %s on line %d
int(1)

Warning: Undefined variable $a (this will become an error in PHP 9.0) in %s on line %d

Warning: Undefined variable $b (this will become an error in PHP 9.0) in %s on line %d
int(0)

Warning: Undefined variable $a (this will become an error in PHP 9.0) in %s on line %d
NULL

Warning: Undefined variable $b (this will become an error in PHP 9.0) in %s on line %d
int(1)

Warning: Attempt to read property "1" on int in %s on line %d
NULL

Warning: Attempt to read property "1" on int in %s on line %d
NULL

Warning: Undefined variable $c (this will become an error in PHP 9.0) in %s on line %d

Warning: Trying to access array offset on value of type null in %s on line %d

Warning: Attempt to read property "1" on int in %s on line %d

Warning: Attempt to read property "" on null in %s on line %d
NULL
