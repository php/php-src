--TEST--
SCCP propagation into the constant name of a dynamic class constant fetch must allocate cache slots
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0x7FFEBBFF
--EXTENSIONS--
opcache
--FILE--
<?php

class A {
    const CONSTANT = "AX";
    public $prop = 2;

    function func() {
        $name = "CONSTANT";
        $x = $this->prop;
        return [$x, static::{$name}];
    }

    static function both_const() {
        $c = "A";
        $name = "CONSTANT";
        return $c::{$name};
    }
}

$a = new A;
var_dump($a->func(), A::both_const());

?>
--EXPECT--
array(2) {
  [0]=>
  int(2)
  [1]=>
  string(2) "AX"
}
string(2) "AX"
