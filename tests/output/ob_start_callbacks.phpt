--TEST--
Test ob_start() with callbacks in variables
--FILE--
<?php

// Closure in variable
$a = function ($s) { return strtoupper($s); };
ob_start($a);
echo 'closure in variable', "\n";
ob_end_flush();

// Object (array) in variable
class foo {
    static function out($foo) {
        return strtoupper($foo);
    }
}
$a = array('foo', 'out');
ob_start($a);
echo 'object in variable', "\n";
ob_end_flush();

// Object with static array
ob_start(array('foo', 'out'));
echo 'object via static array', "\n";
ob_end_flush();

function my_strtoupper($foo, $bar) {
    return strtoupper($foo);
}
$a = 'my_strtoupper';
ob_start($a);
echo 'function via variable', "\n";
ob_end_flush();
?>
--EXPECT--
CLOSURE IN VARIABLE
OBJECT IN VARIABLE
OBJECT VIA STATIC ARRAY
FUNCTION VIA VARIABLE
