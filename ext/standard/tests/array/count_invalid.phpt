--TEST--
Only arrays and countable objects can be counted
--FILE--
<?php

$result = count(null);
var_dump($result);

$result = count("string");
var_dump($result);

$result = count(123);
var_dump($result);

$result = count(true);
var_dump($result);

$result = count(false);
var_dump($result);

$result = count((object) []);
var_dump($result);

?>
--EXPECTF--
Warning: count(): Parameter must be an array or an object that implements Countable in %s on line %d
int(0)

Warning: count(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

Warning: count(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

Warning: count(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

Warning: count(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)

Warning: count(): Parameter must be an array or an object that implements Countable in %s on line %d
int(1)
