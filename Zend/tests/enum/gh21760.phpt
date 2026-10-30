--TEST--
GH-21760 (Trait with class constant name conflict against enum case causes SEGV)
--FILE--
<?php

trait X {
    public const Up = 1;
}

enum Direction {
    use X;

    case Up;
    case Down;
}

?>
--EXPECTF--
Fatal error: Cannot use trait X, because X::Up conflicts with enum case Direction::Up in %s on line %d
