--TEST--
array_udiff() and array_uintersect() preserve results across nested and replaced callbacks
--FILE--
<?php
foreach (['array_udiff', 'array_uintersect'] as $operation) {
    echo "$operation\n";
    $left = ['a' => 3, 'b' => 1, 'c' => 2, 'd' => 3];
    $expected = $operation === 'array_udiff' ? ['a' => 3, 'b' => 1, 'd' => 3] : ['c' => 2];

    // Nested comparisons must restore the outer callback.
    $called = false;
    $result = $operation($left, [2, 4], [2, 5], function ($a, $b) use (&$called) {
        if (!$called) {
            $called = true;
            $inner = [3, 1, 2];
            usort($inner, 'strcmp');
            array_udiff($inner, [2], 'strcmp');
            array_uintersect($inner, [2], 'strcmp');
        }
        return $a <=> $b;
    });
    var_dump($result === $expected);

    // Failed callable validation clears the cached function.
    $called = false;
    $result = $operation($left, [2, 4], [2, 5], function ($a, $b) use (&$called) {
        if (!$called) {
            $called = true;
            try {
                array_udiff_assoc([1], [1], 'no_such_function');
            } catch (TypeError $e) {
                echo $e::class, "\n";
            }
        }
        return $a <=> $b;
    });
    var_dump($result === $expected);

    // A nested associative comparison can leave a different callback cached.
    $called = false;
    $result = $operation($left, [2, 4], [2, 5], function ($a, $b) use (&$called) {
        if (!$called) {
            $called = true;
            array_uintersect_assoc(['1'], ['1'], 'strcmp');
        }
        return $a <=> $b;
    });
    var_dump($result === $expected);

    try {
        $operation($left, [2], static function ($a, $b): int {
            throw new Exception('expected');
        });
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($operation($left, [2, 4], [2, 5], strcmp(...)) === $expected);
}
?>
--EXPECT--
array_udiff
bool(true)
TypeError
bool(true)
bool(true)
expected
bool(true)
array_uintersect
bool(true)
TypeError
bool(true)
bool(true)
expected
bool(true)
