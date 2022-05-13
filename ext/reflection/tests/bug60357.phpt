--TEST--
Bug #60357 (__toString() method triggers E_NOTICE "Array to string conversion")
--FILE--
<?php
function foo(
    array $x = array('a', 'b'),
    array $y = ['x' => 'y'],
    array $z = [0 => 0, 2 => -2],
    array $a = [[], [1], [2, 3]],
) {}
echo new ReflectionFunction('foo'), "\n";
?>
--EXPECTF--
Function [ <user> function foo ] {
  @@ %s

  - Parameters [4] {
    Parameter #0 [ <optional> array $x = ['a', 'b'] ]
    Parameter #1 [ <optional> array $y = ['x' => 'y'] ]
    Parameter #2 [ <optional> array $z = [0 => 0, 2 => -2] ]
    Parameter #3 [ <optional> array $a = [[], [1], [2, 3]] ]
  }
}
