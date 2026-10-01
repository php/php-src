--TEST--
Bug #49269 (Ternary operator fails on Iterator object when used inside foreach declaration).
--FILE--
<?php
class TestObject implements Iterator
{
    public $n = 0;
    function valid(): bool
    {
        return ($this->n < 3);
    }
    function current(): mixed {return $this->n;}
    function next(): void {$this->n++;}
    function key(): mixed { }
    function rewind(): void {$this->n = 0;}
}


$array_object = new TestObject();

foreach ((true ? $array_object : $array_object) as $item) echo "$item\n";
?>
--EXPECT--
0
1
2
