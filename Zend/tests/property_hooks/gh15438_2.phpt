--TEST--
GH-15438: Untyped promoted, hooked properties with no default value default to null
--FILE--
<?php

class C {
    public function __construct(
        public $prop { set => $value * 2; }
    ) {}
}

$c = new ReflectionClass(C::class)->newInstanceWithoutConstructor();
var_dump($c);

?>
--EXPECTF--
object(C)#%d (1) {
  ["prop"]=>
  NULL
}
