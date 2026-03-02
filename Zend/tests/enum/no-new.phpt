--TEST--
Enum no new
--FILE--
<?php

enum Foo {}

try {
    new Foo();
} catch (\Error $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Cannot instantiate enum Foo
