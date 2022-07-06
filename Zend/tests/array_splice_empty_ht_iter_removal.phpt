--TEST--
HT iterator should be destroyed if array becomes empty during array_splice
--FILE--
<?php
$a=[4];
$i = 0;
foreach ($a as &$r) {
    var_dump($r);
    $a = array_splice($a, 0);
    if (++$i == 2) break;
}
?>
--EXPECT--
int(4)
int(4)
