--TEST--
Sort functions: a comparator that suspends a fiber keeps its own callback
--FILE--
<?php

echo "Suspend and resume in a single sort\n";
$fiber = new Fiber(function () {
    $array = [3, 1, 2];
    usort($array, function ($x, $y) {
        Fiber::suspend();
        return $x <=> $y;
    });
    return $array;
});
$fiber->start();
while (!$fiber->isTerminated()) {
    $fiber->resume();
}
echo implode(',', $fiber->getReturn()), "\n";

echo "Another sort runs while the first one is suspended\n";
$fiber = new Fiber(function () {
    $array = [3, 1, 2];
    usort($array, function ($x, $y) {
        echo "A\n";
        Fiber::suspend();
        return $x <=> $y;
    });
    echo "A done\n";
});
$fiber->start();
$array = [5, 4, 6];
$resumed = false;
usort($array, function ($x, $y) use ($fiber, &$resumed) {
    echo "B\n";
    if (!$resumed) {
        $resumed = true;
        $fiber->resume();
    }
    return $x <=> $y;
});
echo implode(',', $array), "\n";

echo "The suspended fiber is destroyed by another sort\n";
$fiber = new Fiber(function () {
    $array = [3, 1, 2];
    usort($array, function ($x, $y) {
        Fiber::suspend();
        return $x <=> $y;
    });
});
$fiber->start();
$array = [5, 4, 6, 1, 2];
usort($array, function ($x, $y) use (&$fiber) {
    $fiber = null;
    return $x <=> $y;
});
echo implode(',', $array), "\n";

echo "Two array_multisort() calls suspend in __toString()\n";
class Item
{
    public function __construct(private string $value) {}

    public function __toString(): string
    {
        Fiber::suspend();
        return $this->value;
    }
}
$first = new Fiber(function () {
    $array = [new Item('b'), new Item('a'), new Item('c')];
    array_multisort($array, SORT_STRING);
    echo implode(',', array_map('strval', $array)) === 'a,b,c' ? "first sorted\n" : "first wrong\n";
});
$second = new Fiber(function () {
    $array = [new Item('f'), new Item('e'), new Item('d')];
    array_multisort($array, SORT_STRING);
    echo implode(',', array_map('strval', $array)) === 'd,e,f' ? "second sorted\n" : "second wrong\n";
});
$first->start();
$second->start();
while (!$first->isTerminated()) {
    $first->resume();
}
while (!$second->isTerminated()) {
    $second->resume();
}

echo "A sort nested in a comparator\n";
$array = [3, 1, 2];
usort($array, function ($x, $y) {
    $inner = [2, 1];
    usort($inner, fn($a, $b) => $a <=> $b);
    return $x <=> $y;
});
echo implode(',', $array), "\n";

?>
--EXPECT--
Suspend and resume in a single sort
1,2,3
Another sort runs while the first one is suspended
A
B
A
B
B
4,5,6
The suspended fiber is destroyed by another sort
1,2,4,5,6
Two array_multisort() calls suspend in __toString()
first sorted
second sorted
A sort nested in a comparator
1,2,3
