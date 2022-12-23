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
    unset($foo->bar);
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
unset(bar): __unset(bar), Unset
Unsetting bar
unset(bar): __unset(bar), Unset
Setting bar
unset(bar): __unset(bar), bar
Unsetting bar
unset(bar): __unset(bar), Unset
