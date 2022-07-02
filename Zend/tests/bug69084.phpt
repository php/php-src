--TEST--
Bug #69084: Unclear error message when not implementing a renamed abstract trait function
--FILE--
<?php

trait Foo {
    abstract public function doStuff();

    public function main() {
        $this->doStuff();
    }
}

class Bar {
    use Foo {
        Foo::doStuff as doOtherStuff;
    }

    public function doStuff() {
        var_dump(__FUNCTION__);
    }
}

$b = new Bar();
$b->main();

?>
--EXPECTF--
Fatal error: Class Bar contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (Bar::doOtherStuff) in %s on line %d
