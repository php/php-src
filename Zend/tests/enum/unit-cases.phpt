--TEST--
Unit enums can list cases
--FILE--
<?php

enum Suit {
    case Hearts;
    case Diamonds;
    case Clubs;
    case Spades;
    /** @deprecated Typo, use Suit::Hearts */
    const Hearst = self::Hearts;
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
