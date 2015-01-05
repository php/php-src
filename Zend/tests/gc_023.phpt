--TEST--
GC 023: Root buffer overflow (automatic collection)
--INI--
zend.enable_gc=1
--FILE--
<?php
$a=array();
for ($i=0; $i < 9999; $i++) {
	$a[$i] = array(array());
	$a[$i][0] = & $a[$i];
}
var_dump(gc_collect_cycles());
unset($a);
var_dump(gc_collect_cycles());
$a=array();
for ($i=0; $i < 10001; $i++) {
	$a[$i] = array(array());
	$a[$i][0] = & $a[$i];
}
var_dump(gc_collect_cycles());
unset($a); // 10000 zvals collected automatic
var_dump(gc_collect_cycles());
echo "ok\n";
--EXPECT--
int(0)
int(9999)
int(0)
int(1)
ok
