--TEST--
GH-25404 (SPL methods forward by-reference returns of user methods)
--FILE--
<?php

class Demo implements RecursiveIterator {
    public mixed $hasChildren = true;
    public mixed $children = null;

    public function &getChildren(): ?RecursiveIterator { return $this->children; }
    public function &hasChildren(): bool { return $this->hasChildren; }

    public function current(): mixed { return null; }
    public function key(): mixed { return null; }
    public function next(): void {}
    public function rewind(): void {}
    public function valid(): bool { return true; }
}

class Filter extends RecursiveFilterIterator {
    public function accept(): bool { return true; }
}

$demo = new Demo();

$it = new RecursiveIteratorIterator($demo);
var_dump($it->callHasChildren());
var_dump($it->callGetChildren());

$filter = new Filter($demo);
var_dump($filter->hasChildren());

$result = $it->callHasChildren();
$result = false;
var_dump($demo->hasChildren);

?>
--EXPECT--
bool(true)
NULL
bool(true)
bool(true)
