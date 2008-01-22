--TEST--
GC  013: Too many cycles in one array
--FILE--
<?php
$a = array();
for ($i = 0; $i < 10001; $i++) {
	$a[$i] =& $a;
}
$a[] = "xxx";
unset($a);
var_dump(gc_collect_cycles());
echo "ok\n";
?>
--EXPECT--
int(2)
ok
