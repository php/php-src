--TEST--
Test array_sum()
--FILE--
<?php
$old_memory_limit = ini_get("memory_limit");
ini_set("memory_limit","16M");
$i = 0;
while ($i++ < 1000) {
	$a[] = $i;
	$b[] = (string)$i;
}
$s1 = array_sum($a);
$s2 = array_sum($b);
var_dump($s1, $s2);

$j = 0;
while ($j++ < 100000) {
	$c[] = $j;
	$d[] = (string) $j;
}
$s3 = array_sum($c);
$s4 = array_sum($d);
var_dump($s3, $s4);
ini_set("memory_limit",$old_memory_limit);
?>
--EXPECT--
int(500500)
int(500500)
float(5000050000)
float(5000050000)