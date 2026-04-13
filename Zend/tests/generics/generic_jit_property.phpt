--TEST--
Generic class: JIT + typed property assignment enforcement
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_func=1
--FILE--
<?php
declare(strict_types=1);

class Container<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
}

// 1. Basic property access
$c = new Container<int>(42);
echo "1. " . $c->value . "\n";

// 2. Valid property assignment
$c->value = 99;
echo "2. " . $c->value . "\n";

// 3. Invalid property assignment
try {
    $c->value = "hello";
} catch (TypeError $e) {
    echo "3. TypeError OK\n";
}

// 4. Hot loop — property assignment under JIT
$box = new Container<int>(0);
for ($i = 0; $i < 1000; $i++) {
    $box->value = $i;
}
echo "4. hot prop: " . $box->value . "\n";

// 5. After JIT, type errors still caught on property
$box2 = new Container<string>("init");
for ($i = 0; $i < 200; $i++) {
    $box2->value = "val$i";
}

try {
    $box2->value = [1, 2]; // array can't coerce to string
} catch (TypeError $e) {
    echo "5. post-jit prop TypeError OK\n";
}

// 6. Inferred type — property enforcement
$inferred = new Container(3.14);
for ($i = 0; $i < 200; $i++) {
    $inferred->value = (float)$i;
}

try {
    $inferred->value = "not a float";
} catch (TypeError $e) {
    echo "6. inferred prop TypeError OK\n";
}

echo "Done.\n";
?>
--EXPECT--
1. 42
2. 99
3. TypeError OK
4. hot prop: 999
5. post-jit prop TypeError OK
6. inferred prop TypeError OK
Done.
