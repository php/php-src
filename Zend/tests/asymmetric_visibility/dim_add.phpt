--TEST--
Asymmetric visibility DIM add
--FILE--
<?php

class Foo {
    public private(set) array $bars = [];

    public function addBar($bar) {
        $this->bars[] = $bar;
    }
}

$foo = new Foo();

try {
    $foo->bars[] = 'baz';
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($foo->bars);

$foo->addBar('baz');
var_dump($foo->bars);

?>
--EXPECT--
Cannot indirectly modify private(set) property Foo::$bars from global scope
array(0) {
}
array(1) {
  [0]=>
  string(3) "baz"
}
