--TEST--
Bug #42798 (Autoloading not triggered for classes used in method signature)
--FILE--
<?php
autoload_register_class(function ($className) {
    print "$className\n";
    exit();
});

function foo($c = ok::constant) {
}

foo();
?>
--EXPECT--
ok
