--TEST--
Using parent::$prop::get() in class with no parent
--FILE--
<?php

class Foo {
    public $prop {
        get => parent::$prop::get();
    }
}

$foo = new Foo();
try {
    var_dump($foo->prop);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot use "parent" when current class scope has no parent
