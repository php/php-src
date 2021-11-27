--TEST--
(Un)Registering autoloaders must take effect immidiately
--FILE--
<?php

function inner_autoload ($name){
    if ($name == 'foo') {
        function foo() { echo "okey, "; }
    } else {
        function bar() { die("error"); }
    }
}

autoload_register_function(function ($name) {
    if ($name == 'foo') {
        autoload_register_function("inner_autoload");
    } else {
        autoload_unregister_function("inner_autoload");
    }
});

$f = foo();
try {
    $f = bar();
} catch (Error $e) {
    echo "done";
}
?>
--EXPECT--
okey, done
