--TEST--
Closure 010: Closure calls itself
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
