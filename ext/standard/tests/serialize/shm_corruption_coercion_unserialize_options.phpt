--TEST--
Shm corruption with coercion in options of unserialize()
--FILE--
<?php
class MyStringable {
    public function __toString(): string {
        return "0";
    }
}

unserialize("{}", ["allowed_classes" => [new MyStringable]]);
?>
--EXPECTF--
Warning: unserialize(): Error at offset 0 of 2 bytes in %s on line %d
