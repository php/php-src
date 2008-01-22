--TEST--
GC  014: Too many cycles in one object
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
int(10002)
ok
