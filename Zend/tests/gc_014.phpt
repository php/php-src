--TEST--
GC 014: Too many cycles in one object
--INI--
zend.enable_gc=1
--FILE--
<?php
$a = new stdClass();
for ($i = 0; $i < 10001; $i++) {
	$b =& $a;
	$a->{"a".$i} = $a;
}
unset($b);
$a->b = "xxx";
unset($a);
var_dump(gc_collect_cycles());
echo "ok\n";
?>
--EXPECT--
int(2)
ok
