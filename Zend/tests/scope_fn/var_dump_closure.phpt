--TEST--
var_dump of scope function closure
--FILE--
<?php
function test() {
    $fn = fn($a) { return $a; };
    var_dump($fn);
}
test();
?>
--EXPECTF--
object(Closure)#%d (%d) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(%d)
  ["parameter"]=>
  array(1) {
    ["$a"]=>
    string(10) "<required>"
  }
}
