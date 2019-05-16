--TEST--
Bug #71013 (Incorrect exception handler with yield from)
--FILE--
<?php

class FooBar implements Iterator {
    function __construct()   { echo "Constructing new FooBar\n"; }
    function __destruct()    { echo "Destructing FooBar\n"; }
    function current ()      { throw new Exception; }
    function key ()          { return 0; }
    function next ()         {}
    function rewind ()       {}
    function valid ()        { return true; }
}

function foo() {
    try {
        $f = new FooBar;
        yield from $f;
    } catch (Exception $e) {
        echo "[foo()] Caught Exception\n";
    }
}

function bar() {
    echo "Starting bar()\n";
    $x = foo();
    try {
        var_dump($x->current());
    } catch (Exception $e) {
        echo "[bar()] Caught Exception\n";
    }
    echo "Unsetting \$x\n";
    unset($x);
    echo "Finishing bar()\n";
}

bar();

?>
--EXPECT--
Starting bar()
Constructing new FooBar
[foo()] Caught Exception
Destructing FooBar
NULL
Unsetting $x
Finishing bar()
