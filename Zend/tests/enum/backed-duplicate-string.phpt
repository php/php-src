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

try {
    var_dump(Suit::Hearts);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(Suit::Hearts);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(Suit::from(42));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(Suit::tryFrom('bar'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Duplicate value in enum Suit for cases Hearts and Spades
Error: Duplicate value in enum Suit for cases Hearts and Spades
Error: Duplicate value in enum Suit for cases Hearts and Spades
Error: Duplicate value in enum Suit for cases Hearts and Spades
