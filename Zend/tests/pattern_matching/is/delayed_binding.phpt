--TEST--
Delayed binding
--FILE--
<?php

class Pair {
    public function __construct(public $a, public $b) {}
}

var_dump(new Pair(1, 2) is Pair(a: $a, b: $b));
var_dump($a, $b);
unset($a, $b);

var_dump(new Pair(1, 2) is Pair(a: $a, b: 3));
var_dump($a, $b);
unset($a, $b);

var_dump(new Pair(new \stdClass(), 2) is Pair(a: $a, b: 2));
var_dump($a, $b);
unset($a, $b);

var_dump(new Pair(new \stdClass(), 2) is Pair(a: $a, b: 3));
var_dump($a, $b);
unset($a, $b);

?>
--EXPECTF--
bool(true)
int(1)
int(2)
bool(false)

Warning: Undefined variable $a in %s on line %d

Warning: Undefined variable $b in %s on line %d
NULL
NULL
bool(true)

Warning: Undefined variable $b in %s on line %d
object(stdClass)#2 (0) {
}
NULL
bool(false)

Warning: Undefined variable $a in %s on line %d

Warning: Undefined variable $b in %s on line %d
NULL
NULL
