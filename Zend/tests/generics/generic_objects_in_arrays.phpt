--TEST--
Generic class: generic objects stored in arrays and GC behavior
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
}

// 1. Array of generic objects
$boxes = [];
for ($i = 0; $i < 5; $i++) {
    $boxes[] = new Box<int>($i);
}

echo "1. count: " . count($boxes) . "\n";
echo "1. first: " . $boxes[0]->value . "\n";
echo "1. last: " . $boxes[4]->value . "\n";

// 2. Mixed generic types in array
$mixed = [
    new Box<int>(42),
    new Box<string>("hello"),
    new Box<float>(3.14),
];

echo "2. int: " . $mixed[0]->value . "\n";
echo "2. str: " . $mixed[1]->value . "\n";
echo "2. float: " . $mixed[2]->value . "\n";

// 3. Unset — should free generic_args properly
unset($boxes[0]);
unset($boxes[1]);
echo "3. after unset, count: " . count($boxes) . "\n";

// 4. Array functions work
$mapped = array_map(fn(Box $b) => $b->value * 2, array_values(array_filter($boxes)));
echo "4. mapped: " . implode(", ", $mapped) . "\n";

// 5. Replace elements
$boxes[0] = new Box<int>(100);
echo "5. replaced: " . $boxes[0]->value . "\n";

// 6. Nested array of generics
$nested = [
    [new Box<int>(1), new Box<int>(2)],
    [new Box<string>("a"), new Box<string>("b")],
];
echo "6. nested: " . $nested[0][0]->value . ", " . $nested[1][1]->value . "\n";

// 7. Type enforcement still works after array storage
try {
    $mixed[0]->value = "not int";
} catch (TypeError $e) {
    echo "7. TypeError OK\n";
}

echo "Done.\n";
?>
--EXPECT--
1. count: 5
1. first: 0
1. last: 4
2. int: 42
2. str: hello
2. float: 3.14
3. after unset, count: 3
4. mapped: 4, 6, 8
5. replaced: 100
6. nested: 1, b
7. TypeError OK
Done.
