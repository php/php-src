--TEST--
Bug #50816 (Using class constants in array definition fails)
--FILE--
<?php
define("ONE", 1);
define("TWO", 1);

class Foo {
  const ONE = 1;
  const TWO = 1;

  public static $mapWithConst = array(self::ONE => 'one', self::TWO => 'two',);

  public static $mapWithConst1 = array(1 => 'one', self::TWO => 'two',);
  public static $mapWithConst2 = array(self::ONE => 'one', 1 => 'two',);

  public static $mapWithoutConst = array(1 => 'one', 1 => 'two',);
}

$mapWithConst = array(1 => 'one', 1 => 'two',);

$mapWithoutConst = array(Foo::ONE => 'one', Foo::TWO => 'two',);
$mapWithoutConst0 = array(1 => 'one', 1 => 'two',);
$mapWithoutConst1 = array(ONE => 'one', 1 => 'two',);
$mapWithoutConst2 = array(1 => 'one', TWO => 'two',);
$mapWithoutConst3 = array(ONE => 'one', TWO => 'two',);

var_dump(Foo::$mapWithConst[1]);
var_dump(Foo::$mapWithConst1[1]);
var_dump(Foo::$mapWithConst2[1]);
var_dump(Foo::$mapWithoutConst[1]);
var_dump($mapWithConst[1]);
var_dump($mapWithoutConst[1]);
var_dump($mapWithoutConst0[1]);
var_dump($mapWithoutConst1[1]);
var_dump($mapWithoutConst2[1]);
var_dump($mapWithoutConst3[1]);
--EXPECT--
string(3) "two"
string(3) "two"
string(3) "two"
string(3) "two"
string(3) "two"
string(3) "two"
string(3) "two"
string(3) "two"
string(3) "two"
string(3) "two"
