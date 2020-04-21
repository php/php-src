--TEST--
Tests autoloading from a classmap does not interupt autoloader if it is not set
--FILE--
<?php

    spl_autoload_register(function($class_id) {
        if ($class_id === 'Bar') {
            echo "Including Bar from autoloader\n";
            require_once __DIR__ . '/Bar.php';
        }
    });

    new Bar();

    echo 'Done';

?>
--EXPECT--
Including Bar from autoloader
Done
