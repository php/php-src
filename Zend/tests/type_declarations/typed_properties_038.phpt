--TEST--
Test typed properties overflowing
--FILE--
<?php

$foo = new class {
    public int $bar = PHP_INT_MAX;
};

try {
    $foo->bar++;
} catch(TypeError $t) {
    var_dump($t->getMessage());
}

var_dump($foo);

try {
    $foo->bar += 1;
} catch(TypeError $t) {
    var_dump($t->getMessage());
}

var_dump($foo);

try {
    ++$foo->bar;
} catch(TypeError $t) {
    var_dump($t->getMessage());
}

var_dump($foo);

try {
    $foo->bar = $foo->bar + 1;
} catch(TypeError $t) {
    var_dump($t->getMessage());
}

var_dump($foo);
?>
--EXPECTF--
string(82) "Cannot increment property class@anonymous::$bar of type int past its maximal value"
object(class@anonymous)#1 (1) {
  ["bar"]=>
  int(%d)
}
string(65) "Cannot assign float to property class@anonymous::$bar of type int"
object(class@anonymous)#1 (1) {
  ["bar"]=>
  int(%d)
}
string(82) "Cannot increment property class@anonymous::$bar of type int past its maximal value"
object(class@anonymous)#1 (1) {
  ["bar"]=>
  int(%d)
}
string(65) "Cannot assign float to property class@anonymous::$bar of type int"
object(class@anonymous)#1 (1) {
  ["bar"]=>
  int(%d)
}
