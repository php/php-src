--TEST--
Packed sorting relocates the internal pointer before invoking comparison callbacks
--FILE--
<?php
class PointerReader {
    public static ?Closure $read = null;
    public function __construct(public int $value) {}
    public function __toString(): string {
        $read = self::$read;
        self::$read = null;
        if ($read) {
            $read();
        }
        return (string) $this->value;
    }
}

foreach (['sort', 'rsort'] as $sort) {
    $values = [];
    foreach ([5, 4, 3, 2, 1] as $value) {
        $values[] = new PointerReader($value);
    }
    next($values);
    next($values);
    next($values);
    unset($values[0], $values[1]);
    PointerReader::$read = static function () use (&$values, $sort) {
        echo "$sort during: ", current($values)->value, "\n";
    };
    $sort($values, SORT_STRING);
    echo "$sort after: ", current($values)->value, "\n";
}
?>
--EXPECT--
sort during: 2
sort after: 1
rsort during: 2
rsort after: 3
