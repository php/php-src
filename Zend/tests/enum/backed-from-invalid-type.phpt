--TEST--
BackedEnum::from() reject invalid type
--FILE--
<?php

enum Suit: string {
    case Hearts = 'H';
    case Diamonds = 'D';
    case Clubs = 'C';
    case Spades = 'S';
}

try {
    var_dump(Suit::from(42));
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

enum Foo: int {
    case Bar = 0;
    case Baz = 1;
}

try {
    var_dump(Foo::from('H'));
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}


?>
--EXPECT--
ValueError: "42" is not a valid backing value for enum Suit
TypeError: Foo::from(): Argument #1 ($value) must be of type int, string given
