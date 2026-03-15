--TEST--
Generic class: generic class defined in included file
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
declare(strict_types=1);

// Define generic class in a temp file and include it
$tmpDir = sys_get_temp_dir();
$file1 = $tmpDir . '/generic_box_' . getmypid() . '.php';
$file2 = $tmpDir . '/generic_pair_' . getmypid() . '.php';

file_put_contents($file1, '<?php
class Box<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}
');

file_put_contents($file2, '<?php
class Pair<A, B> {
    public function __construct(private A $first, private B $second) {}
    public function first(): A { return $this->first; }
    public function second(): B { return $this->second; }
}
');

// Include both files
require_once $file1;
require_once $file2;

// 1. Use included generic class
$box = new Box<int>(42);
echo "1. " . $box->get() . "\n";

// 2. Type enforcement from included class
try {
    $box2 = new Box<int>("bad");
} catch (TypeError $e) {
    echo "2. TypeError OK\n";
}

// 3. Second included generic class
$pair = new Pair<string, int>("age", 25);
echo "3. " . $pair->first() . ": " . $pair->second() . "\n";

// 4. Inheritance from included generic class
class IntBox extends Box<int> {}
$ib = new IntBox(99);
echo "4. " . $ib->get() . "\n";

// 5. Type inference from included class
$inferred = new Box("hello");
echo "5. " . $inferred->get() . "\n";

// Cleanup
unlink($file1);
unlink($file2);

echo "Done.\n";
?>
--EXPECT--
1. 42
2. TypeError OK
3. age: 25
4. 99
5. hello
Done.
