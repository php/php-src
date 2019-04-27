--TEST--
Test typed properties ignore private props during inheritance
--FILE--
<?php
class Foo {
	private int $thing;
}

class Bar extends Foo {
    public string $thing; // No conflict
}

echo "ok";
?>
--EXPECT--
ok
