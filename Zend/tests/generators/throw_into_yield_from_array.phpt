--TEST--
Throwing into a generator yielding from an array/iterator
--FILE--
<?php

$data = [1, 2, 3];

function yielditer($arr) {
    foreach($arr as $val) {
        yield $val;
    }
}

function yf($in) {
    yield from $in;
}

function test($g) {
    var_dump($g->current());
    try {
        $g->throw(new Exception("Exception!"));
    } catch (Exception $e) {
        echo "{$e->getMessage()}\n";
    }
    var_dump($g->current());
}

$yfiter = yf($data);
$yfgen = yf(yielditer($data));

test(yf($data));
echo "\n";
test(yf(yielditer($data)));

?>
--EXPECT--
int(1)
Exception!
NULL

int(1)
Exception!
NULL
