--TEST--
Test typed properties overflowing
--FILE--
<?php

$foo = new class {
    public int $bar = PHP_INT_MAX;
};

try {
    $foo->bar++;
} catch(Throwable $t) {
    echo $t::class, ': ', $t->getMessage(), "\n";
}

var_dump($foo);

try {
    $foo->bar += 1;
} catch(Throwable $t) {
    echo $t::class, ': ', $t->getMessage(), "\n";
}

var_dump($foo);

try {
    ++$foo->bar;
} catch(Throwable $t) {
    echo $t::class, ': ', $t->getMessage(), "\n";
}

var_dump($foo);

try {
    $foo->bar = $foo->bar + 1;
} catch(Throwable $t) {
    echo $t::class, ': ', $t->getMessage(), "\n";
}

var_dump($foo);
?>
--EXPECTF--
TypeError: Cannot increment property class@anonymous::$bar of type int past its maximal value
object(class@anonymous)#1 (1) {
  ["bar"]=>
  int(%d)
}
TypeError: Cannot assign float to property class@anonymous::$bar of type int
object(class@anonymous)#1 (1) {
  ["bar"]=>
  int(%d)
}
TypeError: Cannot increment property class@anonymous::$bar of type int past its maximal value
object(class@anonymous)#1 (1) {
  ["bar"]=>
  int(%d)
}
TypeError: Cannot assign float to property class@anonymous::$bar of type int
object(class@anonymous)#1 (1) {
  ["bar"]=>
  int(%d)
}
