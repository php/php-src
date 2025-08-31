--TEST--
Enum implements
--FILE--
<?php

interface Colorful {
    public function color(): string;
}

enum Suit implements Colorful {
    case Hearts;
    case Diamonds;
    case Clubs;
    case Spades;

    public function color(): string {
        return match ($this) {
            self::Hearts, self::Diamonds => 'Red',
            self::Clubs, self::Spades => 'Black',
        };
    }
}

echo Suit::Hearts->color() . "\n";
echo Suit::Diamonds->color() . "\n";
echo Suit::Clubs->color() . "\n";
echo Suit::Spades->color() . "\n";

?>
--EXPECT--
Red
Red
Black
Black
