--TEST--
Generic static method calls
--FILE--
<?php
declare(strict_types=1);

class Factory<T> {
    public static function create(T $value): T {
        return $value;
    }

    public static function wrap(T $value): array {
        return [$value];
    }
}

// Basic static generic call
$result = Factory<int>::create(42);
echo $result . "\n";  // 42

// String type
$result2 = Factory<string>::wrap("hello");
var_dump($result2);

// Type enforcement: should throw TypeError
try {
    Factory<int>::create("not an int");
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

// Multiple type params
class Converter<TIn, TOut> {
    public static function convert(TIn $input): string {
        return "converted: " . $input;
    }
}

echo Converter<int, string>::convert(42) . "\n";

// Static method with constraint
class Processor<T: Countable> {
    public static function count(T $item): int {
        return count($item);
    }
}

echo Processor<ArrayObject>::count(new ArrayObject([1, 2, 3])) . "\n";

echo "OK\n";
?>
--EXPECTF--
42
array(1) {
  [0]=>
  string(5) "hello"
}
TypeError: %s
converted: 42
3
OK
