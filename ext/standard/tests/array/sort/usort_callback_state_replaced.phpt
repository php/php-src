--TEST--
usort() keeps working when a comparator replaces or clears the shared callback state
--FILE--
<?php
class SuspendingComparator {
    public function __call(string $name, array $args): int {
        Fiber::suspend();
        return $args[0] <=> $args[1];
    }
}

// A failed callable check clears the cached function.
$items = [3, 1, 2];
$called = false;
usort($items, function ($a, $b) use (&$called): int {
    if (!$called) {
        $called = true;
        try {
            array_udiff_assoc([1], [1], 'no_such_function');
        } catch (TypeError $error) {
            echo $error::class, "\n";
        }
    }
    return $a <=> $b;
});
echo implode(',', $items), "\n";

// array_uintersect_assoc() leaves an internal function in the cache.
$items = [3, 1, 2];
$called = false;
usort($items, function ($a, $b) use (&$called): int {
    if (!$called) {
        $called = true;
        array_uintersect_assoc(['1'], ['1'], 'strcmp');
    }
    return $a <=> $b;
});
echo implode(',', $items), "\n";

// A fiber suspended inside another sort's magic comparator leaves that sort's state behind.
$items = [3, 1, 2];
$fiber = null;
try {
    usort($items, function ($a, $b) use (&$fiber): int {
        if ($fiber === null) {
            $fiber = new Fiber(static function (): void {
                $inner = [2, 1];
                usort($inner, [new SuspendingComparator(), 'compare']);
            });
            $fiber->start();
            var_dump($fiber->isSuspended());
        }
        return $a <=> $b;
    });
} catch (FiberError) {
    // The outer sort may call the suspended sort's comparator outside its fiber.
}
echo "done\n";
?>
--EXPECT--
TypeError
1,2,3
1,2,3
bool(true)
done
