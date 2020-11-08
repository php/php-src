--TEST--
array_splice() function precerve foreach by reference iterator pointer
--FILE--
<?php
/* remove before */
$done = 0;
$a = [0,1,2,3,4];
foreach($a as &$v) {
    echo "$v\n";
    if (!$done && $v == 3) {
        $done = 1;
        array_splice($a, 1, 2);
    }
}
echo "\n";

/* remove after */
$done = 0;
$a = [0,1,2,3,4];
foreach($a as &$v) {
    echo "$v\n";
    if (!$done && $v == 0) {
        $done = 1;
        array_splice($a, 2, 2);
    }
}
echo "\n";

/* remove current */
$done = 0;
$a = [0,1,2,3,4];
foreach($a as &$v) {
    echo "$v\n";
    if (!$done && $v == 2) {
        $done = 1;
        array_splice($a, 1, 3);
    }
}
echo "\n";

$replacement = ['x', 'y', 'z'];

/* replace before */
$done = 0;
$a = [0,1,2,3,4];
foreach($a as &$v) {
    echo "$v\n";
    if (!$done && $v == 3) {
        $done = 1;
        array_splice($a, 1, 2, $replacement);
    }
}
echo "\n";

/* replace after */
$done = 0;
$a = [0,1,2,3,4];
foreach($a as &$v) {
    echo "$v\n";
    if (!$done && $v == 0) {
        $done = 1;
        array_splice($a, 2, 2, $replacement);
    }
}
echo "\n";

/* replace current */
$done = 0;
$a = [0,1,2,3,4];
foreach($a as &$v) {
    echo "$v\n";
    if (!$done && $v == 2) {
        $done = 1;
        array_splice($a, 1, 3, $replacement);
    }
}
echo "\n";
?>
--EXPECT--
0
1
2
3
4

0
1
4

0
1
2
4

0
1
2
3
4

0
1
x
y
z
4

0
1
2
4
