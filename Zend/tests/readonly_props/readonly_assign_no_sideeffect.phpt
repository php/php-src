--TEST--
Test that there can be no side-effect when a readonly property modification fails
--FILE--
<?php

class Foo {
    public function __construct(
        public readonly string $bar
    ) {}

    public function write() {
        $this->bar = new S();
    }
}

class S {
    public function __toString() {
        var_dump("Side-effect in __toString()");
        return "";
    }
}

$foo = new Foo("");

try {
    $foo->write();
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Cannot modify readonly property Foo::$bar
