--TEST--
BackedEnum::from() reject invalid string
--FILE--
<?php

enum Suit: string {
    case Hearts = 'H';
    case Diamonds = 'D';
    case Clubs = 'C';
    case Spades = 'S';
}

try {
    var_dump(Suit::from('A'));
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
"A" is not a valid backing value for enum Suit
