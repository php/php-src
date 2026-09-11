--TEST--
Packed scalar sorting keeps its buffer alive when NAN coercion invokes an error handler
--FILE--
<?php
foreach ([2, 17, 1025] as $size) {
    foreach (['sort', 'rsort'] as $sort) {
        foreach ([SORT_STRING, SORT_STRING | SORT_FLAG_CASE,
                  SORT_NATURAL, SORT_NATURAL | SORT_FLAG_CASE] as $flag) {
            $values = array_fill(0, $size, NAN);
            $warned = false;
            set_error_handler(function ($severity, $message) use (&$values, &$warned) {
                if ($severity !== E_WARNING
                        || $message !== 'unexpected NAN value was coerced to string') {
                    throw new Exception($message);
                }
                $warned = true;
                $values = ['replacement'];
                gc_collect_cycles();
                return true;
            });
            try {
                $result = $sort($values, $flag);
            } finally {
                restore_error_handler();
            }
            if (!$result || !$warned || $values !== ['replacement']) {
                throw new Exception('Lost error-handler mutation');
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
