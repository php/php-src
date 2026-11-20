--TEST--
Native sorting callbacks retain conversions, exceptions and bound method context
--FILE--
<?php
$items = ['30', '2', '10'];
usort($items, strnatcmp(...));
echo implode(',', $items), "\n";

$items = [30 => 'a', 'item2' => 'b', 2 => 'c'];
uksort($items, 'strnatcmp');
echo implode(',', array_keys($items)), ': ', implode(',', $items), "\n";

class SortValue {
    public function __construct(public int $value) {}
    public function compare(self $other): int {
        return $this->value <=> $other->value;
    }
}
$method = new ReflectionMethod(SortValue::class, 'compare');
foreach ([[$method, 'invoke'], $method->invoke(...)] as $callback) {
    $items = [new SortValue(3), new SortValue(1), new SortValue(2)];
    usort($items, $callback);
    foreach ($items as $item) { echo $item->value; }
    echo "\n";
}

foreach (['strcmp', strcmp(...), 'strlen'] as $callback) {
    try {
        $items = [[], []];
        usort($items, $callback);
    } catch (TypeError $error) {
        echo $error::class, ': ', $error->getMessage(), "\n";
    }
}
echo "done\n";
?>
--EXPECT--
2,10,30
2,30,item2: c,a,b
123
123
TypeError: strcmp(): Argument #1 ($string1) must be of type string, array given
TypeError: strcmp(): Argument #1 ($string1) must be of type string, array given
ArgumentCountError: strlen() expects exactly 1 argument, 2 given
done
