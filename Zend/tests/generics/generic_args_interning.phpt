--TEST--
Generic class: generic args interning deduplicates identical type arguments
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

// Many instances with the same type args should work correctly (interning deduplicates)
$boxes = [];
for ($i = 0; $i < 100; $i++) {
    $boxes[] = new Box<int>($i);
}
echo "Created 100 Box<int>\n";

// Verify all instances preserve correct values
for ($i = 0; $i < 100; $i++) {
    if ($boxes[$i]->get() !== $i) {
        echo "FAIL: Box[$i] has wrong value\n";
        exit;
    }
}
echo "All values correct\n";

// Inferred args should also be interned
$inferred = [];
for ($i = 0; $i < 50; $i++) {
    $inferred[] = new Box($i);
}
for ($i = 0; $i < 50; $i++) {
    if ($inferred[$i]->get() !== $i) {
        echo "FAIL: inferred Box[$i] has wrong value\n";
        exit;
    }
}
echo "Inferred args correct\n";

// Different type args must remain distinct
$intBox = new Box<int>(42);
$strBox = new Box<string>("hello");
$floatBox = new Box<float>(3.14);

echo $intBox->get() . "\n";
echo $strBox->get() . "\n";
echo $floatBox->get() . "\n";

// Type enforcement still works after interning
try {
    $intBox->value = "wrong";
} catch (TypeError $e) {
    echo "int box: caught\n";
}

try {
    $strBox->value = 123;
} catch (TypeError $e) {
    echo "str box: caught\n";
}

// Multi-param generic args interning
class Pair<T, U> {
    public function __construct(public T $first, public U $second) {}
}

$pairs = [];
for ($i = 0; $i < 50; $i++) {
    $pairs[] = new Pair<int, string>($i, "v$i");
}
echo "Created 50 Pair<int, string>\n";

for ($i = 0; $i < 50; $i++) {
    if ($pairs[$i]->first !== $i || $pairs[$i]->second !== "v$i") {
        echo "FAIL: Pair[$i] has wrong values\n";
        exit;
    }
}
echo "All pairs correct\n";

// Expanded defaults interning: DefaultMap<int> expands to DefaultMap<int, string>
class DefaultMap<T, U = string> {
    public function __construct(public T $key, public U $val) {}
}

// Explicit 2 args — no expansion
$m1 = new DefaultMap<int, string>(1, "a");
$m2 = new DefaultMap<int, string>(2, "b");
echo $m1->key . ":" . $m1->val . "\n";
echo $m2->key . ":" . $m2->val . "\n";

// Only 1 arg — V defaults to string, triggers expansion + interning
$m3 = new DefaultMap<int>(3, "c");
$m4 = new DefaultMap<int>(4, "d");
echo $m3->key . ":" . $m3->val . "\n";
echo $m4->key . ":" . $m4->val . "\n";

// Generic function inference produces interned args
function identity<T>(T $v): T { return $v; }
echo identity(42) . "\n";
echo identity("hello") . "\n";

// Context resolution interning: new Box<T> inside a generic factory
class Factory<T> {
    public static function make(T $val): Box<T> {
        return new Box<T>($val);
    }
}

$fb1 = Factory<int>::make(100);
$fb2 = Factory<int>::make(200);
echo $fb1->get() . "\n";
echo $fb2->get() . "\n";

echo "OK\n";
?>
--EXPECT--
Created 100 Box<int>
All values correct
Inferred args correct
42
hello
3.14
int box: caught
str box: caught
Created 50 Pair<int, string>
All pairs correct
1:a
2:b
3:c
4:d
42
hello
100
200
OK
