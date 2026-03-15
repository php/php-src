--TEST--
Generic class: type inference with multiple type parameters
--FILE--
<?php
declare(strict_types=1);

class Pair<A, B> {
    public function __construct(private A $first, private B $second) {}
    public function getFirst(): A { return $this->first; }
    public function getSecond(): B { return $this->second; }
    public function setFirst(A $value): void { $this->first = $value; }
    public function setSecond(B $value): void { $this->second = $value; }
}

// Infer A=int, B=string
$p = new Pair(1, "hello");
echo $p->getFirst() . "\n";
echo $p->getSecond() . "\n";

// Should TypeError: A was inferred as int
try {
    $p->setFirst("bad");
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

// Should TypeError: B was inferred as string
try {
    $p->setSecond(42);
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

echo "OK\n";
?>
--EXPECTF--
1
hello
Pair::setFirst(): Argument #1 ($value) must be of type int, string given, called in %s on line %d
Pair::setSecond(): Argument #1 ($value) must be of type string, int given, called in %s on line %d
OK
