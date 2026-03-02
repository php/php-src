--TEST--
GH-17916: Abstract property cannot be marked as final
--FILE--
<?php

abstract class Foo {
    final abstract public string $bar {
        get;
    }
}

?>
--EXPECTF--
Fatal error: Cannot use the final modifier on an abstract property in %s on line %d
