--TEST--
Generic class: instanceof with generic type arguments
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
}

$intBox = new Box<int>(42);
$strBox = new Box<string>("hello");

// instanceof without generic args (erasure) — always true for correct base class
var_dump($intBox instanceof Box);       // true
var_dump($strBox instanceof Box);       // true

// instanceof with matching generic args
var_dump($intBox instanceof Box<int>);     // true
var_dump($strBox instanceof Box<string>);  // true

// instanceof with non-matching generic args
var_dump($intBox instanceof Box<string>);  // false
var_dump($strBox instanceof Box<int>);     // false

echo "OK\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
OK
