--TEST--
Packed zval sorting keeps its buffer alive when conversions modify the sorted variable
--FILE--
<?php
function check($actual, $expected) {
    if ($actual !== $expected) {
        throw new Exception(var_export([$actual, $expected], true));
    }
}
class ReentrantString {
    public static ?Closure $action = null;
    public static int $destroyed = 0;
    public function __construct(public int $value) {}
    public function __toString(): string {
        $action = self::$action;
        self::$action = null;
        if ($action) $action();
        return (string) $this->value;
    }
    public function __destruct() { self::$destroyed++; }
}
function makeValues(int $size) {
    $values = [];
    for ($i = $size; $i > 0; $i--) $values[] = new ReentrantString($i);
    return $values;
}

foreach ([2, 16, 32, 64, 1025] as $size) {
    foreach (['sort', 'rsort'] as $sort) {
        $values = makeValues($size);
        $original = $values;
        ReentrantString::$action = function () use (&$values) { $values[] = 'appended'; };
        check($sort($values, SORT_STRING), true);
        check($values, [...$original, 'appended']);
        unset($values, $original);

        $values = makeValues($size);
        ReentrantString::$action = function () use (&$values) { $values = ['replacement']; };
        check($sort($values, SORT_STRING), true);
        check($values, ['replacement']);

        $values = makeValues($size);
        $weak = WeakReference::create($values[0]);
        ReentrantString::$destroyed = 0;
        ReentrantString::$action = function () use (&$values) { $values = null; gc_collect_cycles(); };
        check($sort($values, SORT_STRING), true);
        check($values, null);
        check($weak->get(), null);
        check(ReentrantString::$destroyed, $size);

        $values = makeValues($size);
        $expected = $values;
        $expected[0] = 'changed';
        $referenceSort = $sort === 'sort' ? 'asort' : 'arsort';
        $referenceSort($expected, SORT_STRING);
        ReentrantString::$action = function () use (&$values, $sort) {
            $values[0] = 'changed';
            $sort($values, SORT_STRING);
        };
        check($sort($values, SORT_STRING), true);
        check($values, array_values($expected));
        unset($values, $expected);

        $values = makeValues($size);
        $ids = array_map(spl_object_id(...), $values);
        ReentrantString::$action = function () { throw new RuntimeException('conversion'); };
        try {
            $sort($values, SORT_STRING);
            throw new Exception('Missing exception');
        } catch (RuntimeException $e) {
            check($e->getMessage(), 'conversion');
        }
        $resultIds = array_map(spl_object_id(...), $values);
        sort($ids);
        sort($resultIds);
        check($resultIds, $ids);
        check(array_is_list($values), true);
        unset($values);

        $values = array_fill(0, $size, []);
        $first = true;
        set_error_handler(function () use (&$values, &$first) {
            if ($first) {
                $first = false;
                $values = ['error handler'];
                gc_collect_cycles();
            }
            return true;
        });
        try {
            check($sort($values, SORT_STRING), true);
        } finally {
            restore_error_handler();
        }
        check($first, false);
        check($values, ['error handler']);
        echo "$size $sort: OK\n";
    }
}
?>
--EXPECT--
2 sort: OK
2 rsort: OK
16 sort: OK
16 rsort: OK
32 sort: OK
32 rsort: OK
64 sort: OK
64 rsort: OK
1025 sort: OK
1025 rsort: OK
