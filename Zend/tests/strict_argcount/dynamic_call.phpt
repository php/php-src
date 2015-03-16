--TEST--
Exceeding arg count check should not affect callbacks or dispatched function calls
--FILE--
<?php

function variableCallback($a, $b) {
    echo __FUNCTION__, PHP_EOL;    
}

function fn(callable $callback) {
    $callback(1);
    $callback(1, 2, 3);
    $callback(1, 2, 3, 4, 5, 6);
}

fn('variableCallback');

class Dispatched {
    public function someMethod($a, $b){
        echo __METHOD__, PHP_EOL; 
    }

    public static function someStaticMethod($a, $b){
        echo __METHOD__, PHP_EOL; 
    }
}

$prefix = "some";
$method = $prefix . "Method";

(new Dispatched)->$method(1);
(new Dispatched)->$method(1, 2);
(new Dispatched)->$method(1, 2, 3);

$prefix = "someStatic";
$method = $prefix . "Method";

Dispatched::$method(1);
Dispatched::$method(1, 2);
Dispatched::$method(1, 2, 3);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--

Warning: Missing argument 2 for variableCallback(), called in %s on line 8 and defined in %s on line 3
variableCallback
variableCallback
variableCallback

Warning: Missing argument 2 for Dispatched::someMethod(), called in %s on line 28 and defined in %s on line 16
Dispatched::someMethod
Dispatched::someMethod
Dispatched::someMethod

Warning: Missing argument 2 for Dispatched::someStaticMethod(), called in %s on line 35 and defined in %s on line 20
Dispatched::someStaticMethod
Dispatched::someStaticMethod
Dispatched::someStaticMethod

Done
