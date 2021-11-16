--TEST--
Backed enums reject duplicate string values
--FILE--
<?php

enum Suit: string {
    case Hearts = 'H';
    case Diamonds = 'D';
    case Clubs = 'C';
    case Spades = 'H';
}

?>
--EXPECTF--
Fatal error: Duplicate value in enum Suit for cases Hearts and Spades in %s on line %s
