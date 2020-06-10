--TEST--
BackedEnum::tryFrom()
--FILE--
<?php

enum Suit: string {
    case Hearts = 'H';
    case Diamonds = 'D';
    case Clubs = 'C';
    case Spades = 'S';
}

var_dump(Suit::tryFrom('H'));
var_dump(Suit::tryFrom('D'));
var_dump(Suit::tryFrom('C'));
var_dump(Suit::tryFrom('S'));
var_dump(Suit::tryFrom('X'));

enum Foo: int {
    case Bar = 1;
    case Baz = 2;
    case Beep = 3;
}

var_dump(Foo::tryFrom(1));
var_dump(Foo::tryFrom(2));
var_dump(Foo::tryFrom(3));
var_dump(Foo::tryFrom(4));

?>
--EXPECT--
enum(Suit::Hearts)
enum(Suit::Diamonds)
enum(Suit::Clubs)
enum(Suit::Spades)
NULL
enum(Foo::Bar)
enum(Foo::Baz)
enum(Foo::Beep)
NULL
