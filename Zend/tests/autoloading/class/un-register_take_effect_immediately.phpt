--TEST--
(Un)Registering autoloaders must take effect immidiately
--FILE--
<?php

function inner_autoload ($name){
    if ($name == 'A') {
        class A {
            function __construct(){
                echo "okey, ";
            }
        }
    } else {
        class B {
            function __construct() {
                die("error");
            }
        }
    }
}

autoload_register_class(function ($name) {
    if ($name == 'A') {
        autoload_register_class("inner_autoload");
    } else {
        autoload_unregister_class("inner_autoload");
    }
});

$c = new A();
try {
    $c = new B();
} catch (Error $e) {
    echo "done";
}
?>
--EXPECT--
okey, done
