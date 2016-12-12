--TEST--
Bug #71539.1 (Memory error on $arr[$a] =& $arr[$b] if RHS rehashes)
--FILE--
<?php
$x = (object)['a'=>1,'b'=>2,'c'=>3,'d'=>4,'e'=>5,'f'=>6,'g'=>7];
$x->h =& $x->i;
$x->h = 42;
var_dump($x);
?>
--EXPECT--
object(stdClass)#1 (9) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  ["d"]=>
  int(4)
  ["e"]=>
  int(5)
  ["f"]=>
  int(6)
  ["g"]=>
  int(7)
  ["i"]=>
  &int(42)
  ["h"]=>
  &int(42)
}
