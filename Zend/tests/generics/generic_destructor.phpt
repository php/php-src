--TEST--
Generic class: destructor runs with generic_args intact
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public T $value;

    public function __construct(T $value) {
        $this->value = $value;
    }

    public function __destruct() {
        // generic_args should still be accessible during destruction
        echo "destroying Box with value: " . $this->value . "\n";
    }
}

// 1. Normal destruction at end of scope
function test_scope(): void {
    $b = new Box<int>(42);
    echo "1. created\n";
}
test_scope();

// 2. Explicit unset
$b = new Box<string>("hello");
echo "2. before unset\n";
unset($b);
echo "2. after unset\n";

// 3. Replacement triggers destruction
$b = new Box<int>(1);
echo "3. before replace\n";
$b = new Box<int>(2);
echo "3. after replace\n";

// 4. Destruction order in array
$arr = [
    new Box<int>(10),
    new Box<int>(20),
];
echo "4. before array unset\n";
unset($arr);
echo "4. after array unset\n";

echo "Done.\n";
?>
--EXPECT--
1. created
destroying Box with value: 42
2. before unset
destroying Box with value: hello
2. after unset
3. before replace
destroying Box with value: 1
3. after replace
4. before array unset
destroying Box with value: 10
destroying Box with value: 20
4. after array unset
Done.
destroying Box with value: 2
