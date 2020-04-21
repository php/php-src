--TEST--
Tests autoloading from a classmap still allows autoloader to run
--FILE--
<?php

    autoload_classmap([
        Foo::class => __DIR__ . '/Foo.php'
    ]);

    spl_autoload_register(function($class_id) {
        if ($class_id === 'Bar') {
            echo "Including Bar from autoloader\n";
            require_once __DIR__ . '/Bar.php';
        }
    });

    new Bar();

    echo 'Done';

?>
--EXPECTF--
Notice: Cannot find bar in the autoload classmap in %s on line %d
Including Bar from autoloader
Done
