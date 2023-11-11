--TEST--
Enum cannot have properties, even via traits
--FILE--
<?php

trait Rectangle {
    protected string $shape = "Rectangle";

    public function shape(): string {
        return $this->shape;
    }
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
Fatal error: Enum Suit cannot include properties in %s on line %d
