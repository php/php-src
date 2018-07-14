--TEST--
iterable type#006 - Iterating stdClass by reference
--FILE--
<?php

$test = (object) ["one" => 1, "two" => 2];

foreach ($test as &$value) {
   ++$value;
}
unset($value);

var_dump($test);

--EXPECT--
 object(stdClass)#1 (2) {
  ["one"]=>
  int(2)
  ["two"]=>
  int(3)
}