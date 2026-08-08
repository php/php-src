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
string(%d) "Cannot increment property class@anonymous%0%s:3$%x::$bar of type int past its maximal value"
object(class@anonymous)#1 (1) {
  ["bar"]=>
  int(%d)
}
string(%d) "Cannot assign float to property class@anonymous%0%s:3$%x::$bar of type int"
object(class@anonymous)#1 (1) {
  ["bar"]=>
  int(%d)
}
string(%d) "Cannot increment property class@anonymous%0%s:3$%x::$bar of type int past its maximal value"
object(class@anonymous)#1 (1) {
  ["bar"]=>
  int(%d)
}
string(%d) "Cannot assign float to property class@anonymous%0%s:3$%x::$bar of type int"
object(class@anonymous)#1 (1) {
  ["bar"]=>
  int(%d)
}
