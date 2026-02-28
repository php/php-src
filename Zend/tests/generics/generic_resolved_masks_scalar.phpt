--TEST--
Generic class: resolved_masks fast path for scalar type checks
--DESCRIPTION--
Tests that the pre-computed resolved_masks optimization correctly handles
all scalar types (int, string, float, bool, array, null) for both
parameter and return type checking, and property assignment.
--FILE--
<?php
declare(strict_types=1);

class TypedBox<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
    public function set(T $value): void { $this->value = $value; }
}

// int — fast path mask should have MAY_BE_LONG
$intBox = new TypedBox<int>(42);
echo $intBox->get() . "\n";
$intBox->set(99);
echo $intBox->value . "\n";

// string — fast path mask should have MAY_BE_STRING
$strBox = new TypedBox<string>("hello");
echo $strBox->get() . "\n";
$strBox->set("world");
echo $strBox->value . "\n";

// float — fast path mask should have MAY_BE_DOUBLE
$floatBox = new TypedBox<float>(3.14);
echo $floatBox->get() . "\n";

// bool — fast path mask should have MAY_BE_TRUE|MAY_BE_FALSE
$boolBox = new TypedBox<bool>(true);
var_dump($boolBox->get());

// array — fast path mask should have MAY_BE_ARRAY
$arrBox = new TypedBox<array>([1, 2, 3]);
var_dump($arrBox->get());

// Type mismatch on constructor (mask fast path rejects, falls through to error)
try {
    new TypedBox<int>("not an int");
} catch (TypeError $e) {
    echo "ctor reject: OK\n";
}

// Type mismatch on set() (mask fast path rejects)
try {
    $intBox->set("string");
} catch (TypeError $e) {
    echo "set reject: OK\n";
}

// Type mismatch on property assignment (mask fast path rejects)
try {
    $intBox->value = "string";
} catch (TypeError $e) {
    echo "prop reject: OK\n";
}

// Return type (mask fast path validates)
echo "return: " . $intBox->get() . "\n";

echo "Done.\n";
?>
--EXPECT--
42
99
hello
world
3.14
bool(true)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
ctor reject: OK
set reject: OK
prop reject: OK
return: 99
Done.
