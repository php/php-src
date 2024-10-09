--TEST--
Int backed enums with can list cases
--FILE--
<?php

enum Suit: int {
    case Hearts;
    case Diamonds;
    case Clubs;
    case Spades;
}

var_dump(Suit::cases());

?>
--EXPECTF--
Fatal error: Case Hearts of backed enum Suit must have a value in %s on line %d
