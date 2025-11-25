--TEST--
Object pattern matching destructor
--FILE--
<?php

class Foo {
    public function __destruct() {
        throw new Exception('Here');
    }
}

$foo = new Foo();
$bar = 'bar';

try {
    42 is $foo & $bar;
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($foo);
var_dump($bar);

?>
--EXPECT--
Exception: Here
int(42)
string(3) "bar"
