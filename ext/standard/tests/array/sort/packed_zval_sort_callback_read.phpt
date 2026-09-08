--TEST--
Implicit sorting callbacks can read packed array indices at every array size
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
            ReadDuringSort::$read = function () use (&$values, &$reads, $size) {
                if (!$values[0] instanceof ReadDuringSort
                    || !$values[$size - 1] instanceof ReadDuringSort) {
                    throw new Exception('Missing numeric index during comparison');
                }
                $reads++;
            };
            $sort($values, $flags);
            if (!$reads || !array_is_list($values)) throw new Exception('No reads or invalid keys');
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
