--TEST--
Generic class: constraint with class type bound (valid usage)
--DESCRIPTION--
Tests that generic type parameter constraints properly enforce that
type arguments implement the required interface. This test covers
valid constraint satisfaction. Constraint violations trigger E_ERROR
which is tested separately.
--FILE--
<?php
declare(strict_types=1);

interface Printable {
    public function toString(): string;
}

class PrintableInt implements Printable {
    public function __construct(private int $val) {}
    public function toString(): string { return (string)$this->val; }
}

class PrintableString implements Printable {
    public function __construct(private string $val) {}
    public function toString(): string { return $this->val; }
}

// Generic class with constraint
class Printer<T: Printable> {
    private T $item;
    public function __construct(T $item) { $this->item = $item; }
    public function print(): string { return $this->item->toString(); }
}

// 1. Valid constraint satisfaction
$p1 = new Printer<PrintableInt>(new PrintableInt(42));
echo "1. " . $p1->print() . "\n";

$p2 = new Printer<PrintableString>(new PrintableString("hello"));
echo "1. " . $p2->print() . "\n";

// 2. Generic function with constraint
function stringify<T: Printable>(T $item): string {
    return $item->toString();
}

echo "2. " . stringify(new PrintableInt(99)) . "\n";
echo "2. " . stringify(new PrintableString("world")) . "\n";

// 3. Inheritance with constrained parent
class SpecialPrinter extends Printer<PrintableInt> {
    public function printDouble(): string {
        return $this->print() . $this->print();
    }
}

$sp = new SpecialPrinter(new PrintableInt(5));
echo "3. " . $sp->printDouble() . "\n";

// 4. Multiple constrained instances
$printers = [];
for ($i = 0; $i < 5; $i++) {
    $printers[] = new Printer<PrintableInt>(new PrintableInt($i));
}
$parts = [];
foreach ($printers as $p) {
    $parts[] = $p->print();
}
echo "4. " . implode(" ", $parts) . "\n";

echo "Done.\n";
?>
--EXPECT--
1. 42
1. hello
2. 99
2. world
3. 55
4. 0 1 2 3 4
Done.
