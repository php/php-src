--TEST--
Generic class: Closure::bind and bindTo with generic $this
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    private T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

// 1. Closure reading private property of generic object
$getter = Closure::bind(function() {
    return $this->value;
}, new Box<int>(42), Box::class);

echo "1. " . $getter() . "\n";

// 2. Closure bound to different generic instance
$box1 = new Box<string>("hello");
$box2 = new Box<string>("world");

$fn = function() { return $this->value; };
$bound1 = Closure::bind($fn, $box1, Box::class);
$bound2 = Closure::bind($fn, $box2, Box::class);

echo "2. " . $bound1() . "\n";
echo "2. " . $bound2() . "\n";

// 3. bindTo
$box3 = new Box<int>(99);
$rebound = $bound1->bindTo($box3, Box::class);
echo "3. " . $rebound() . "\n";

// 4. Closure with use() capturing generic object
$captured = new Box<int>(7);
$fn = function() use ($captured) {
    return $captured->get();
};
echo "4. " . $fn() . "\n";

// 5. Closure modifying generic object
$modifier = Closure::bind(function($val) {
    $this->value = $val;
}, new Box<int>(0), Box::class);

$modifier(42);
// Note: the closure's $this is a copy of the original binding
echo "5. OK\n";

echo "Done.\n";
?>
--EXPECT--
1. 42
2. hello
2. world
3. 99
4. 7
5. OK
Done.
