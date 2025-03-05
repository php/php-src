--TEST--
Basic test
--FILE--
<?php

class FOO {
    public function __toString(): string {
        return "hello world";
    }
}

class BAR() extends FOO;

echo (new BAR());
?>
--EXPECT--
hello world
