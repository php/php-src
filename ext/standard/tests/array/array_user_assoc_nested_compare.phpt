--TEST--
array_udiff_assoc() and array_uintersect_assoc() restore the user comparison callback
--FILE--
<?php

$sorted = [3, 1, 2];
usort($sorted, function ($a, $b) {
    echo "outer\n";
    array_udiff_assoc([1], [1], function ($x, $y) {
        echo "inner\n";
        return $x <=> $y;
    });
    return $a <=> $b;
});
echo implode(',', $sorted), "\n";

$sorted = [3, 1, 2];
usort($sorted, function ($a, $b) {
    echo "outer\n";
    array_uintersect_assoc([1], [1], function ($x, $y) {
        echo "inner\n";
        return $x <=> $y;
    });
    return $a <=> $b;
});
echo implode(',', $sorted), "\n";

?>
--EXPECT--
outer
inner
outer
inner
outer
inner
1,2,3
outer
inner
outer
inner
outer
inner
1,2,3
