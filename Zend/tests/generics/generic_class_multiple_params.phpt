--TEST--
Generic class: multiple type parameters
--FILE--
<?php
class Pair<A, B> {
    private $first;
    private $second;
    public function __construct(A $first, B $second) {
        $this->first = $first;
        $this->second = $second;
    }
    public function getFirst(): A { return $this->first; }
    public function getSecond(): B { return $this->second; }
}

$pair = new Pair<string, int>("hello", 42);
echo $pair->getFirst() . "\n";
echo $pair->getSecond() . "\n";
?>
--EXPECT--
hello
42
