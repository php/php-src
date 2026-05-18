--TEST--
Parametric LSP: implementing two distinct generic interfaces with different bindings
--FILE--
<?php
interface Reader<T> {
    public function read(): T;
}

interface Writer<U> {
    public function write(U $value): void;
}

class IntReaderStringWriter implements Reader<int>, Writer<string> {
    private string $w = "";
    public function read(): int { return 42; }
    public function write(string $value): void { $this->w = $value; echo "wrote: $value\n"; }
}

$x = new IntReaderStringWriter;
echo $x->read(), "\n";
$x->write("hello");
?>
--EXPECT--
42
wrote: hello
