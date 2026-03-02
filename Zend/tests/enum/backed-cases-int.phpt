--TEST--
Int backed enums with can list cases
--FILE--
<?php

enum Suit: int {
    case Hearts = 2;
    case Diamonds = 1;
    case Clubs = 4;
    case Spades = 3;
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
