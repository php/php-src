--TEST--
GH-21158: Assertion jit->ra[var].flags & (1<<0) failed in zend_jit_use_reg
--CREDITS--
YuanchengJiang
--EXTENSIONS--
opcache
--INI--
opcache.jit=1254
--FILE--
<?php
define('ROW', 10);
define('COL', 10);
function count_live_neighbors($board, $row, $col) {
    $live_neighbors = 0;
    if ($col + 1 < COL && $board[$row][$col + 1] == 1) $live_neighbors++;
    if ($row - 1 >= 0 && $col - 1 >= 0 && $board[$row - 1][$col - 1] == 1) $live_neighbors++;
    if ($row >= 1 && $col + 1 < COL && $board[$row - 1][$col + 1] == 1) $live_neighbors++;
    return $live_neighbors;
}
$board = [
    [1,1,0,0,1,1,1,1,1,0],
    [0,1,0,1,1,0,0,1,0,0],
    [0,1,0,0,1,0,0,0,1,0],
    [0,0,1,1,1,1,1,0,0,0],
    [1,1,1,1,1,1,0,1,1,0],
    [0,1,0,0,1,1,1,0,1,0],
    [0,1,1,0,1,1,1,1,0,0],
    [1,1,0,0,0,0,1,1,1,0],
    [1,0,0,1,1,0,1,1,0,1],
    [0,0,1,1,1,0,1,1,0,1],
];
for ($i = 0; $i < ROW; $i++) {
    for ($j = 0; $j < COL; $j++) {
        echo count_live_neighbors($board, $i, $j), ",";
    }
    echo "\n";
}
?>
--EXPECT--
1,0,0,1,1,1,1,1,0,0,
2,1,2,2,1,2,3,2,1,1,
2,0,2,2,1,1,1,1,1,0,
1,1,2,2,1,2,0,1,0,1,
1,2,2,3,3,2,2,2,0,0,
2,2,2,3,3,2,2,2,1,1,
2,1,1,2,2,3,2,2,0,1,
2,1,1,2,1,3,3,2,1,0,
1,1,2,1,0,2,2,2,2,1,
0,2,2,2,1,3,2,1,3,0,
