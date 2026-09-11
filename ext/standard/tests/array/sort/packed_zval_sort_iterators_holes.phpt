--TEST--
Packed sorting compacts holes without losing by-reference foreach positions
--FILE--
<?php
$values = [5, 4, 3, 2, 1];
unset($values[0], $values[1], $values[2]);
$visited = [];
$first = true;
foreach ($values as $key => &$value) {
    $visited[] = "$key=>$value";
    if ($first) {
        $first = false;
        sort($values);
    }
}
unset($value);
echo implode(" ", $visited), "\n";

$values = [9, 8, 7, 6, 5, 4];
unset($values[0], $values[1]);
$visited = [];
$first = true;
foreach ($values as $key => &$value) {
    $visited[] = "$key=>$value";
    if ($first) {
        $first = false;
        rsort($values);
    }
}
unset($value);
echo implode(" ", $visited), "\n";

// A cursor at the old end must pick up an append after compaction.
$values = [3, 2, 1];
unset($values[0], $values[1]);
$visited = [];
$first = true;
foreach ($values as $key => &$value) {
    $visited[] = "$key=>$value";
    if ($first) {
        $first = false;
        sort($values);
        $values[] = 9;
    }
}
unset($value);
echo implode(" ", $visited), "\n";

// Unsetting the current reference leaves an all-integer packed array with a hole.
$values = [3, 2, 1];
$visited = [];
$first = true;
foreach ($values as $key => &$value) {
    $visited[] = "$key=>$value";
    if ($first) {
        $first = false;
        unset($values[$key]);
        sort($values);
    }
}
unset($value);
echo implode(" ", $visited), "\n";
?>
--EXPECT--
3=>2 1=>2
2=>7 1=>6 2=>5 3=>4
2=>1 1=>9
0=>3 0=>1 1=>2
