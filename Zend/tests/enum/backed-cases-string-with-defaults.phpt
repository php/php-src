--TEST--
String backed enums can list cases
--FILE--
<?php

enum Vegetable: string {
    case Carrot;
    case Potato;
    case Tomato;
}

var_dump(Vegetable::cases());
var_dump(Vegetable::Carrot->value);
var_dump(Vegetable::Potato->value);
var_dump(Vegetable::Tomato->value);

enum Fruit: string {
    case Apple;
    case Orange;
    case Pineapple = 'Ananas';
}

var_dump(Fruit::cases());
var_dump(Fruit::Apple->value);
var_dump(Fruit::Orange->value);
var_dump(Fruit::Pineapple->value);

?>
--EXPECT--
array(3) {
  [0]=>
  enum(Vegetable::Carrot)
  [1]=>
  enum(Vegetable::Potato)
  [2]=>
  enum(Vegetable::Tomato)
}
string(6) "Carrot"
string(6) "Potato"
string(6) "Tomato"
array(3) {
  [0]=>
  enum(Fruit::Apple)
  [1]=>
  enum(Fruit::Orange)
  [2]=>
  enum(Fruit::Pineapple)
}
string(5) "Apple"
string(6) "Orange"
string(6) "Ananas"
