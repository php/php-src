--TEST--
Multiple typed constants in one declaration
--FILE--
<?php

class Test {
    public const int X = 1, Y = "foo";
}

?>
--EXPECTF--
Fatal error: Cannot use string as value for class constant Test::Y of type int in %s on line %d
