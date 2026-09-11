--TEST--
Sorting warnings preserve missing-index behavior after a scalar prefix
--FILE--
<?php
foreach ([2, 17, 1025] as $size) {
    foreach (['sort', 'rsort'] as $sort) {
        foreach ([NAN, []] as $value) {
            foreach ([false, true] as $reference) {
                foreach ([SORT_STRING, SORT_STRING | SORT_FLAG_CASE,
                          SORT_NATURAL, SORT_NATURAL | SORT_FLAG_CASE] as $flag) {
                    $values = range($size, 1);
                    $values[$size - 1] = $value;
                    if ($reference) {
                        $alias = &$values[$size - 1];
                    }
                    $expectedWarning = is_array($value)
                        ? 'Array to string conversion'
                        : 'unexpected NAN value was coerced to string';
                    $warnings = 0;
                    set_error_handler(static function ($severity, $message) use (
                        &$values, &$warnings, $expectedWarning, $size
                    ) {
                        if ($severity !== E_WARNING || $message !== $expectedWarning) {
                            throw new Exception($message);
                        }
                        if (isset($values[0]) || isset($values[$size - 1])
                            || array_key_exists(0, $values) || array_key_exists($size - 1, $values)) {
                            throw new Exception('Numeric index became visible to the error handler');
                        }
                        $warnings++;
                        return true;
                    });
                    try {
                        $result = $sort($values, $flag);
                    } finally {
                        restore_error_handler();
                    }
                    if (!$result || !$warnings || count($values) !== $size || !array_is_list($values)) {
                        throw new Exception('Missing warning or invalid result');
                    }
                    unset($alias);
                }
            }
        }
        echo "$size $sort: OK\n";
    }
}
?>
--EXPECT--
2 sort: OK
2 rsort: OK
17 sort: OK
17 rsort: OK
1025 sort: OK
1025 rsort: OK
