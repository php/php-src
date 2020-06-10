--TEST--
BackedEnum::from()
--FILE--
<?php

enum Suit: string {
    case Hearts = 'H';
    case Diamonds = 'D';
    case Clubs = 'C';
    case Spades = 'S';
}

var_dump(Suit::from('H'));
var_dump(Suit::from('D'));
var_dump(Suit::from('C'));
var_dump(Suit::from('S'));

enum Foo: int {
    case Bar = 1;
    case Baz = 2;
    case Beep = 3;
}

var_dump(Foo::from(1));
var_dump(Foo::from(2));
var_dump(Foo::from(3));

?>
--EXPECT--
enum(Suit::Hearts)
enum(Suit::Diamonds)
enum(Suit::Clubs)
enum(Suit::Spades)
enum(Foo::Bar)
enum(Foo::Baz)
enum(Foo::Beep)
