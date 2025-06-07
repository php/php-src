--TEST--
Readonly class Test cannot use trait with a non-readonly property
--FILE--
<?php

trait SomeTrait {
    public int $prop;
}

readonly class Test {
    use SomeTrait;
}

?>
--EXPECTF--
Fatal error: Readonly class Test cannot use trait with a non-readonly property SomeTrait::$prop in %s on line %d
