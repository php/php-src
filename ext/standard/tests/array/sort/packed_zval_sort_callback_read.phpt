--TEST--
Implicit sorting callbacks preserve missing-index behavior at every array size
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
class ReadDuringSort {
    public static ?Closure $read = null;
    public function __construct(public int $value) {}
    public function __toString(): string {
        (self::$read)();
        return (string) $this->value;
    }
}

setlocale(LC_COLLATE, 'C');
foreach ([2, 16, 17, 32, 63, 64, 65, 1025] as $size) {
    foreach (['sort', 'rsort'] as $sort) {
        foreach ([SORT_STRING, SORT_STRING | SORT_FLAG_CASE, SORT_NATURAL,
                  SORT_NATURAL | SORT_FLAG_CASE, SORT_LOCALE_STRING] as $flags) {
            $values = [];
            for ($i = $size; $i > 0; $i--) $values[] = new ReadDuringSort($i);
            $reads = 0;
            $warnings = [];
            set_error_handler(static function ($severity, $message) use (&$warnings) {
                if ($severity !== E_WARNING || !str_starts_with($message, 'Undefined array key ')) {
                    throw new Exception($message);
                }
                $warnings[] = $message;
                return true;
            });
            ReadDuringSort::$read = function () use (&$values, &$reads, $size) {
                if (isset($values[0]) || isset($values[$size - 1])
                    || array_key_exists(0, $values) || array_key_exists($size - 1, $values)) {
                    throw new Exception('Numeric index became visible during comparison');
                }
                if ($reads === 0 && ($values[0] !== null || $values[$size - 1] !== null)) {
                    throw new Exception('Unexpected value during comparison');
                }
                $reads++;
            };
            try {
                $sort($values, $flags);
            } finally {
                restore_error_handler();
            }
            if (!$reads || !array_is_list($values)) throw new Exception('No reads or invalid keys');
            if ($warnings !== ['Undefined array key 0', 'Undefined array key ' . ($size - 1)]) {
                throw new Exception('Missing undefined-key warnings');
            }
            ReadDuringSort::$read = null;
        }
    }
    echo "size $size: OK\n";
}
?>
--EXPECT--
size 2: OK
size 16: OK
size 17: OK
size 32: OK
size 63: OK
size 64: OK
size 65: OK
size 1025: OK
