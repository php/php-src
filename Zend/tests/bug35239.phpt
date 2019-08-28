--TEST--
Bug #35239 (Objects can lose references)
--FILE--
<?php
$a = new StdClass;
$a->x0 = new StdClass;
$a->x0->y0 = 'a';
$a->x0->y1 =& $a->x0;
$a->x0->y2 =& $a->x0;
$a->x0->y0 = 'b';
var_dump($a);
$a->x0->y1 = "ok\n";
echo $a->x0;
?>
--EXPECTF--
object(StdClass)#%d (1) {
  ["x0"]=>
  &object(StdClass)#%d (3) {
    ["y0"]=>
    string(1) "b"
    ["y1"]=>
    *RECURSION*
    ["y2"]=>
    *RECURSION*
  }
}
ok
