--TEST--
Enum can use traits
--FILE--
<?php

trait Rectangle {
    public function shape(): string {
        return "Rectangle";
    }
}

enum Suit {
    use Rectangle;

    case Hearts;
    case Diamonds;
    case Clubs;
    case Spades;
}

echo Suit::Hearts->shape() . PHP_EOL;
echo Suit::Diamonds->shape() . PHP_EOL;
echo Suit::Clubs->shape() . PHP_EOL;
echo Suit::Spades->shape() . PHP_EOL;

?>
--EXPECT--
Rectangle
Rectangle
Rectangle
Rectangle
