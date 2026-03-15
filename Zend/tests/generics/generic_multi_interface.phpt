--TEST--
Generic class: implementing multiple generic interfaces
--FILE--
<?php
declare(strict_types=1);

interface Readable<T> {
    public function read(): T;
}

interface Writable<T> {
    public function write(T $value): void;
}

interface Transformable<T, U> {
    public function transform(T $input): U;
}

// Implements two generic interfaces with same type
class Storage<T> implements Readable<T>, Writable<T> {
    private T $data;
    public function __construct(T $initial) { $this->data = $initial; }
    public function read(): T { return $this->data; }
    public function write(T $value): void { $this->data = $value; }
}

// Implements two generic interfaces with bound types
class StringToIntConverter implements Readable<string>, Transformable<string, int> {
    private string $data = "";
    public function read(): string { return $this->data; }
    public function transform(string $input): int { return strlen($input); }
}

// 1. Storage with same type for both interfaces
$s = new Storage<int>(42);
echo "1. read: " . $s->read() . "\n";
$s->write(99);
echo "1. after write: " . $s->read() . "\n";

try {
    $s->write("not int");
} catch (TypeError $e) {
    echo "1. TypeError OK\n";
}

// 2. Bound types
$conv = new StringToIntConverter();
echo "2. transform: " . $conv->transform("hello") . "\n";
echo "2. read: '" . $conv->read() . "'\n";

// 3. instanceof checks
echo "3. Storage is Readable: " . ($s instanceof Readable ? "yes" : "no") . "\n";
echo "3. Storage is Writable: " . ($s instanceof Writable ? "yes" : "no") . "\n";
echo "3. Converter is Readable: " . ($conv instanceof Readable ? "yes" : "no") . "\n";
echo "3. Converter is Transformable: " . ($conv instanceof Transformable ? "yes" : "no") . "\n";

echo "Done.\n";
?>
--EXPECT--
1. read: 42
1. after write: 99
1. TypeError OK
2. transform: 5
2. read: ''
3. Storage is Readable: yes
3. Storage is Writable: yes
3. Converter is Readable: yes
3. Converter is Transformable: yes
Done.
