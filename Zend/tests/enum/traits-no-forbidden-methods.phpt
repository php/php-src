--TEST--
Enum cannot have forbidden methods, even via traits
--FILE--
<?php

trait Rectangle {
    public function __construct() {}
}

enum Suit {
    use Rectangle;

    case Hearts;
    case Diamonds;
    case Clubs;
    case Spades;
}

?>
--EXPECTF--
Fatal error: Enum Suit cannot include magic method __construct in %s on line %d
