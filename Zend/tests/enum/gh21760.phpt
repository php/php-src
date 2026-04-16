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
Fatal error: Direction and X define the same constant (Up) in the composition of Direction. However, the definition differs and is considered incompatible. Class was composed in %s on line %d
