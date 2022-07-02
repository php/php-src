--TEST--
SPL: iterator_apply()
--FILE--
<?php

function my_error_handler($errno, $errstr, $errfile, $errline) {
    echo "Error: $errstr\n";
}

set_error_handler('my_error_handler');

function test_arg($arg)
{
    if ($arg instanceof Iterator)
    {
        var_dump($arg->key());
        var_dump($arg->current());
    }
    else
    {
        var_dump($arg);
    }
    return true;
}

function test()
{
    static $arg = 0;
    var_dump($arg++);
    return true;
}

$it = new RecursiveArrayIterator(array(1, array(21, 22), 3));

var_dump(iterator_apply($it, 'test', NULL));

echo "===ARGS===\n";
var_dump(iterator_apply($it, 'test_arg', array($it)));

echo "===RECURSIVE===\n";
$it = new RecursiveIteratorIterator($it);
var_dump(iterator_apply($it, 'test'));

echo "===ERRORS===\n";
try {
    var_dump(iterator_apply($it, 'test', 1));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(iterator_apply($it, 'non_existing_function'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(iterator_apply($it, 'non_existing_function', NULL, 2));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
int(0)
int(1)
int(2)
int(3)
===ARGS===
int(0)
int(1)
int(1)
array(2) {
  [0]=>
  int(21)
  [1]=>
  int(22)
}
int(2)
int(3)
int(3)
===RECURSIVE===
int(3)
int(4)
int(5)
int(6)
int(4)
===ERRORS===
iterator_apply(): Argument #3 ($args) must be of type ?array, int given
iterator_apply(): Argument #2 ($callback) must be a valid callback, function "non_existing_function" not found or invalid function name
iterator_apply() expects at most 3 arguments, 4 given
