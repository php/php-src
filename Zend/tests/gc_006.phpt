--TEST--
GC 006: Simple array-object cycle
--FILE--
<?php
$a = new stdClass();
$a->a = array();
$a->a[0] =& $a;
var_dump($a);
unset($a);
var_dump(gc_collect_cycles());
echo "ok\n"
?>
--EXPECT--
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
int(2)
ok
