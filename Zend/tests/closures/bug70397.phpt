--TEST--
Bug #70397 (Segmentation fault when using Closure::call and yield)
--FILE--
<?php

$f = function () {
    $this->value = true;
    yield $this->value;
};

var_dump($f->call(new class {
    public $value;
})->current());

?>
--EXPECT--
bool(true)
