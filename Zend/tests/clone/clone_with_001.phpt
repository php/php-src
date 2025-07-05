--TEST--
Clone with basic
--FILE--
<?php

class Dummy { }

$x = new stdClass();

$foo = 'FOO';
$bar = new Dummy();
$array = [
	'baz' => 'BAZ',
	'array' => [1, 2, 3],
];

var_dump(clone $x);
var_dump(clone($x));
var_dump(clone($x, [ 'foo' => $foo, 'bar' => $bar ]));
var_dump(clone($x, $array));
var_dump(clone($x, [ 'obj' => $x ]));

var_dump(clone($x, [
	'abc',
	'def',
	new Dummy(),
	'named' => 'value',
]));

?>
--EXPECTF--
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
object(stdClass)#%d (2) {
  ["foo"]=>
  string(3) "FOO"
  ["bar"]=>
  object(Dummy)#%d (0) {
  }
}
object(stdClass)#%d (2) {
  ["baz"]=>
  string(3) "BAZ"
  ["array"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
object(stdClass)#%d (1) {
  ["obj"]=>
  object(stdClass)#%d (0) {
  }
}
object(stdClass)#%d (4) {
  ["0"]=>
  string(3) "abc"
  ["1"]=>
  string(3) "def"
  ["2"]=>
  object(Dummy)#%d (0) {
  }
  ["named"]=>
  string(5) "value"
}
