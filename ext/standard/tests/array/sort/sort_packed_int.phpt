--TEST--
sort() and rsort() on packed arrays of integers
--FILE--
<?php
$a = [3, -1, PHP_INT_MAX, 0, PHP_INT_MIN, 3, 2];
var_dump(sort($a));
echo json_encode($a), "\n";
var_dump(rsort($a));
echo json_encode($a), "\n";

echo "\nTwo to six elements, in every order:\n";
function permutations(array $xs): array
{
    if (count($xs) <= 1) {
        return [$xs];
    }
    $out = [];
    foreach ($xs as $i => $x) {
        $rest = $xs;
        unset($rest[$i]);
        foreach (permutations(array_values($rest)) as $p) {
            $out[] = [$x, ...$p];
        }
    }
    return $out;
}
foreach ([[1, 2], [1, 2, 3], [1, 2, 3, 4], [1, 2, 3, 4, 5], [1, 1, 2, 2, 3], [0, 1, 2, 3, 4, 5]] as $set) {
    $ok = true;
    foreach (permutations($set) as $p) {
        $a = $p;
        sort($a);
        $ok = $ok && $a === $set;
        $a = $p;
        rsort($a);
        $ok = $ok && $a === array_reverse($set);
    }
    echo json_encode($set), ': ', $ok ? 'ok' : 'FAILED', "\n";
}

echo "\nHoles are removed, keys renumbered, the internal pointer and next key reset:\n";
$a = [50, 40, 30, 20, 10, 0];
unset($a[0], $a[3]);
end($a);
sort($a);
var_dump($a, key($a));
$a[] = 99;
var_dump(array_key_last($a));

echo "\nThe sorted array is separated from its copies:\n";
$a = [3, 1, 2];
$b = $a;
$c = [$a];
sort($b);
sort($c[0]);
echo json_encode([$a, $b, $c]), "\n";

echo "\nFlags that compare like SORT_REGULAR:\n";
foreach ([SORT_REGULAR, SORT_FLAG_CASE] as $flags) {
    $a = [10, 9, 2, 1];
    sort($a, $flags);
    echo json_encode($a), "\n";
}
?>
--EXPECT--
bool(true)
[-9223372036854775808,-1,0,2,3,3,9223372036854775807]
bool(true)
[9223372036854775807,3,3,2,0,-1,-9223372036854775808]

Two to six elements, in every order:
[1,2]: ok
[1,2,3]: ok
[1,2,3,4]: ok
[1,2,3,4,5]: ok
[1,1,2,2,3]: ok
[0,1,2,3,4,5]: ok

Holes are removed, keys renumbered, the internal pointer and next key reset:
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(10)
  [2]=>
  int(30)
  [3]=>
  int(40)
}
int(0)
int(4)

The sorted array is separated from its copies:
[[3,1,2],[1,2,3],[[1,2,3]]]

Flags that compare like SORT_REGULAR:
[1,2,9,10]
[1,2,9,10]
