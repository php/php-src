--TEST--
Handling of new in constant expressions in reflection
--FILE--
<?php

function test1() {
    static $x = new stdClass;
    return $x;
}

$rf = new ReflectionFunction('test1');
var_dump($rf->getStaticVariables());
test1();

$s = $rf->getStaticVariables();
var_dump($s['x'] === test1());

function test2($x = new stdClass) {
    return $x;
}

$rf = new ReflectionFunction('test2');
$rp = $rf->getParameters()[0];
// Parameter default should *not* be the same.
var_dump($rp->getDefaultValue() !== test2());

?>
--EXPECT--
array(1) {
  ["x"]=>
  NULL
}
bool(true)
bool(true)
