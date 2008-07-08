--TEST--
Closure 010: Closure calls itself
--SKIPIF--
<?php 
	if (!class_exists('Closure')) die('skip Closure support is needed');
?>
--FILE--
<?php
$i = 3;
$lambda = function ($lambda) use (&$i) {
    if ($i==0) return;
    echo $i--."\n";
    $lambda($lambda);
};
$lambda($lambda);
echo "$i\n";
?>
--EXPECT--
3
2
1
0
