--TEST--
GC 005: Simple object cycle
--FILE--
<?php
$a = new stdClass();
$a->a = $a;
var_dump($a);
unset($a);
var_dump(gc_collect_cycles());
echo "ok\n"
?>
--EXPECT--
object(stdClass)#1 (1) {
  [u"a"]=>
  object(stdClass)#1 (1) {
    [u"a"]=>
    *RECURSION*
  }
}
int(1)
ok
