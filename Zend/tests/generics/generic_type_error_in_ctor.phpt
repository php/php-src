--TEST--
Generic class: TypeError during generic constructor
--DESCRIPTION--
Tests that TypeErrors thrown during generic constructor don't
leak memory or leave the object in an inconsistent state.
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public T $value;
    public function __construct(T $value) {
        $this->value = $value;
    }
}

class ValidatedBox<T> {
    public T $value;
    public function __construct(T $value) {
        if (is_int($value) && $value < 0) {
            throw new InvalidArgumentException("Must be non-negative");
        }
        $this->value = $value;
    }
}

// 1. TypeError on explicit wrong type
try {
    $b = new Box<int>("hello");
} catch (TypeError $e) {
    echo "1. TypeError: OK\n";
}

// 2. Custom exception in constructor
try {
    $b = new ValidatedBox<int>(-5);
} catch (InvalidArgumentException $e) {
    echo "2. " . $e->getMessage() . "\n";
}

// 3. Valid construction after failed one
$b = new Box<int>(42);
echo "3. " . $b->value . "\n";

// 4. Multiple failed constructions — no memory leak
for ($i = 0; $i < 100; $i++) {
    try {
        new Box<int>("bad");
    } catch (TypeError $e) {
        // expected
    }
}
echo "4. 100 failures OK\n";

// 5. Failed then successful with same class
try {
    new ValidatedBox<int>(-1);
} catch (InvalidArgumentException $e) {
    // expected
}
$vb = new ValidatedBox<int>(42);
echo "5. " . $vb->value . "\n";

// 6. Multiple types, each fails independently
try {
    new Box<string>([1, 2]); // array is not string
} catch (TypeError $e) {
    echo "6. string TypeError OK\n";
}

try {
    new Box<float>("not a float");
} catch (TypeError $e) {
    echo "6. float TypeError OK\n";
}

echo "Done.\n";
?>
--EXPECT--
1. TypeError: OK
2. Must be non-negative
3. 42
4. 100 failures OK
5. 42
6. string TypeError OK
6. float TypeError OK
Done.
