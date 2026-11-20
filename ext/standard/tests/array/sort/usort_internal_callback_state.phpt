--TEST--
Native sorting callbacks tolerate callback-state changes during string conversion
--FILE--
<?php
class SortString {
    public static bool $clear = false;
    public function __construct(public string $value) {}
    public function __toString(): string {
        if (self::$clear) {
            self::$clear = false;
            try {
                array_udiff_assoc([1], [1], 'no_such_function');
            } catch (TypeError) {
                echo "cleared\n";
            }
        }
        return $this->value;
    }
}
foreach (['strcmp', strcmp(...)] as $callback) {
    SortString::$clear = true;
    $items = [new SortString('3'), new SortString('1'), new SortString('2')];
    usort($items, $callback);
    foreach ($items as $item) { echo $item->value; }
    echo "\n";
}
?>
--EXPECT--
cleared
123
cleared
123
