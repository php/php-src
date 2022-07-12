--TEST--
Asymmetric visibility __unset
--FILE--
<?php

class Foo {
    public private(set) string $bar;

    public function setBar($bar) {
        echo "Setting bar\n";
        $this->bar = $bar;
    }

    public function unsetBar() {
        echo "Unsetting bar\n";
        unset($this->bar);
    }

    public function __unset($name) {
        echo "__unset($name), ";
    }
}

function test($foo) {
    echo "unset(bar): ";
    try {
        unset($foo->bar);
    } catch (Throwable $e) {
        echo $e->getMessage(), ', ';
    }
    echo $foo->bar ?? 'Unset', "\n";
}

$foo = new Foo();
test($foo);

$foo->unsetBar();
test($foo);

$foo->setBar('bar');
test($foo);

$foo->unsetBar();
test($foo);

?>
--EXPECT--
unset(bar): Cannot unset private(set) property Foo::$bar from global scope, Unset
Unsetting bar
unset(bar): __unset(bar), Unset
Setting bar
unset(bar): Cannot unset private(set) property Foo::$bar from global scope, bar
Unsetting bar
unset(bar): __unset(bar), Unset
