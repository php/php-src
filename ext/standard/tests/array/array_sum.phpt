--TEST--
Test array_sum()
--INI--
precision=14
memory_limit=128M
--FILE--
<?php
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
?>
--EXPECTF--
int(500500)
int(500500)
%st(5000050000)
%st(5000050000)
