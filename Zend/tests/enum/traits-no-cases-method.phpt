--TEST--
Using cases method from traits in enums has no effect
--FILE--
<?php

trait Rectangle {
    public static function cases(): array {
        return [];
    }
}

enum Suit {
    use Rectangle;

    case Hearts;
    case Diamonds;
    case Clubs;
    case Spades;
}

var_dump(Suit::cases());

?>
--EXPECT--
array(4) {
  [0]=>
  enum(Suit::Hearts)
  [1]=>
  enum(Suit::Diamonds)
  [2]=>
  enum(Suit::Clubs)
  [3]=>
  enum(Suit::Spades)
}
