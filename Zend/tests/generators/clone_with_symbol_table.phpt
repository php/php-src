--TEST--
A generator using a symbol table can be cloned
--FILE--
<?php

function gen() {
    // force compiled variable for $foo
    $foo = 'foo';

    // force symbol table
    extract(['foo' => 'bar']);

    // interrupt
    yield;

    var_dump($foo);
}

$g1 = gen();
$g1->rewind();
$g2 = clone $g1;
$g1->close();
$g2->next();

?>
--EXPECT--
string(3) "bar"
