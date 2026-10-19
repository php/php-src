--TEST--
Sort functions: suspending a fiber from the comparator throws a FiberError
--FILE--
<?php

function run(string $label, callable $sort)
{
    $fiber = new Fiber($sort);
    try {
        $fiber->start();
        echo "$label: no error\n";
    } catch (FiberError $e) {
        echo "$label: ", $e->getMessage(), "\n";
    }
}

$suspend = function ($x, $y) {
    Fiber::suspend();
    return $x <=> $y;
};

run('usort', function () use ($suspend) {
    $array = [3, 1, 2];
    usort($array, $suspend);
});

run('uasort', function () use ($suspend) {
    $array = [3, 1, 2];
    uasort($array, $suspend);
});

run('uksort', function () use ($suspend) {
    $array = ['b' => 1, 'a' => 2];
    uksort($array, $suspend);
});

run('array_udiff', function () use ($suspend) {
    array_udiff([1, 2], [1], $suspend);
});

run('array_uintersect', function () use ($suspend) {
    array_uintersect([1, 2], [1], $suspend);
});

run('array_diff_ukey', function () use ($suspend) {
    array_diff_ukey(['a' => 1], ['a' => 1], $suspend);
});

run('array_multisort', function () {
    $array = [
        new class {
            function __toString(): string
            {
                Fiber::suspend();
                return '3';
            }
        },
        new class {
            function __toString(): string
            {
                return '1';
            }
        },
    ];
    array_multisort($array, SORT_STRING);
});

$fiber = new Fiber(function () {
    Fiber::suspend('before');
    $array = [3, 1, 2];
    usort($array, fn($x, $y) => $x <=> $y);
    return $array;
});
var_dump($fiber->start());
$fiber->resume();
var_dump($fiber->getReturn());

$array = [3, 1, 2];
try {
    usort($array, function ($x, $y) {
        throw new Exception('boom');
    });
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
$fiber = new Fiber(function () {
    Fiber::suspend('after exception');
});
var_dump($fiber->start());

$array = [3, 1, 2];
usort($array, function ($x, $y) {
    $inner = [2, 1];
    usort($inner, fn($a, $b) => $a <=> $b);
    return $x <=> $y;
});
var_dump($array);
?>
--EXPECT--
usort: Cannot switch fibers in current execution context
uasort: Cannot switch fibers in current execution context
uksort: Cannot switch fibers in current execution context
array_udiff: Cannot switch fibers in current execution context
array_uintersect: Cannot switch fibers in current execution context
array_diff_ukey: Cannot switch fibers in current execution context
array_multisort: Cannot switch fibers in current execution context
string(6) "before"
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
boom
string(15) "after exception"
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
