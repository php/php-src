--TEST--
GC 009: Unreferensed array-object cycle
--FILE--
<?php
$a = array();
$a[0] = new stdClass();
$a[0]->a = array();
$a[0]->a[0] =& $a[0];
var_dump($a[0]);
var_dump(gc_collect_cycles());
unset($a);
var_dump(gc_collect_cycles());
echo "ok\n"
?>
--EXPECT--
object(stdClass)#1 (1) {
  ["a"]=>
  array(1) {
    [0]=>
    &object(stdClass)#1 (1) {
      ["a"]=>
      array(1) {
        [0]=>
        *RECURSION*
      }
    }
  }
}
int(0)
int(2)
ok
--UEXPECT--
object(stdClass)#1 (1) {
  [u"a"]=>
  array(1) {
    [0]=>
    &object(stdClass)#1 (1) {
      [u"a"]=>
      array(1) {
        [0]=>
        *RECURSION*
      }
    }
  }
}
int(0)
int(2)
ok
