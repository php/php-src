--TEST--
Backed Enum with multiple implementing interfaces
--FILE--
<?php

interface Colorful {
    public function color(): string;
}

interface Shaped {
    public function shape(): string;
}

interface ExtendedShaped extends Shaped {
}

enum Suit: string implements Colorful, ExtendedShaped {
    case Hearts = 'H';
    case Diamonds = 'D';
    case Clubs = 'C';
    case Spades = 'S';

    public function color(): string {
        return match ($this) {
            self::Hearts, self::Diamonds => 'Red',
            self::Clubs, self::Spades => 'Black',
        };
    }

    public function shape(): string {
        return match ($this) {
            self::Hearts => 'heart',
            self::Diamonds => 'diamond',
            self::Clubs => 'club',
            self::Spades => 'spade',
        };
    }
}

echo Suit::Hearts->color() . "\n";
echo Suit::Hearts->shape() . "\n";
echo Suit::Diamonds->color() . "\n";
echo Suit::Diamonds->shape() . "\n";
echo Suit::Clubs->color() . "\n";
echo Suit::Clubs->shape() . "\n";
echo Suit::Spades->color() . "\n";
echo Suit::Spades->shape() . "\n";

?>
--EXPECT--
Red
heart
Red
diamond
Black
club
Black
spade
