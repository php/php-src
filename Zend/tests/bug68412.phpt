--TEST--
Bug #68412 (Infinite recursion with __call can make the program crash/segfault)
--FILE--
<?php
class C {
  public function __call($x, $y) {
    global $z;
    $z->bar();
  }
}
$z = new C;
function main() {
  global $z;
  $z->foo();
}
main();
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %sbug68412.php on line %d
