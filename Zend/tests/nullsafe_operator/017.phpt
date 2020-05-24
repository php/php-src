--TEST--
Test return nullsafe as ref
--FILE--
<?php

class Foo {
    public $bar;
}

function &get_bar_ref($foo) {
    return $foo?->bar;
}

function test($foo) {
    $barRef = &get_bar_ref($foo);
    $barRef = 'bar';
    unset($barRef);
    var_dump($foo);

    $barRef = &(strrev('fer_rab_teg'))($foo);
    $barRef = 'bar2';
    unset($barRef);
    var_dump($foo);
}

test(null);
test(new Foo());

?>
--EXPECTF--
Notice: Only variable references should be returned by reference in %s.php on line 8
NULL

Notice: Only variable references should be returned by reference in %s.php on line 8
NULL

Notice: Only variable references should be returned by reference in %s.php on line 8
object(Foo)#1 (1) {
  ["bar"]=>
  NULL
}

Notice: Only variable references should be returned by reference in %s.php on line 8
object(Foo)#1 (1) {
  ["bar"]=>
  NULL
}
