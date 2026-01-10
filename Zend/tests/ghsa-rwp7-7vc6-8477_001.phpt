--TEST--
GHSA-rwp7-7vc6-8477: Use-after-free for ??= due to incorrect live-range calculation
--FILE--
<?php

class Foo {
    public function foo() {
        return $this;
    }

    public function __set($name, $value) {
        throw new Exception('Hello');
    }
}

$foo = new Foo();

try {
    $foo->foo()->baz ??= 1;
} catch (Exception $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Hello
