--TEST--
Packed single-element sorting resets metadata without invoking user code
--FILE--
<?php
// Suppress the constant's PHP 8.6 deprecation before installing the error handler.
$flags = [SORT_REGULAR, SORT_REGULAR | SORT_FLAG_CASE, SORT_NUMERIC,
    SORT_STRING, SORT_STRING | SORT_FLAG_CASE, SORT_NATURAL,
    SORT_NATURAL | SORT_FLAG_CASE, @SORT_LOCALE_STRING, -1];

set_error_handler(static function ($severity, $message) {
    throw new Exception($message);
});

class Value {
    public function __toString(): string {
        throw new Exception('Unexpected comparison');
    }
}

$compare = static function ($a, $b) {
    throw new Exception('Unexpected comparison');
};

foreach (['sort', 'rsort', 'usort'] as $sort) {
    foreach ($flags as $flag) {
        foreach ([null, false, true, 42, 1.5, NAN, INF, 'value', [], new Value] as $value) {
            foreach ([false, true] as $hole) {
                foreach ([false, true] as $reference) {
                    $array = $hole ? [null, $value] : [$value, null];
                    unset($array[$hole ? 0 : 1]);
                    if ($reference) {
                        $alias = &$array[$hole ? 1 : 0];
                    }
                    end($array);
                    next($array);
                    $original = $array;
                    $expected = serialize([$value]);
                    $sort($array, $sort === 'usort' ? $compare : $flag);
                    if (serialize($array) !== $expected || key($array) !== 0
                            || array_keys($original) !== [$hole ? 1 : 0]) {
                        throw new Exception('Incorrect value, cursor, or copy on write');
                    }
                    $array[] = 'appended';
                    if (array_keys($array) !== [0, 1]) {
                        throw new Exception('Incorrect next free index');
                    }
                    unset($alias);
                }
            }
        }
    }
    echo "$sort: OK\n";
}
?>
--EXPECT--
sort: OK
rsort: OK
usort: OK
