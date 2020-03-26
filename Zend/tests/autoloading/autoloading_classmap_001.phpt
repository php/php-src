--TEST--
Tests autoloading from a classmap
--FILE--
<?php

    autoload_classmap([
        Foo::class => __DIR__ . '/Foo.php'
    ]);

    autoload_classmap([
        Bar::class => __DIR__ . '/Bar.php'
    ]);

    new Foo();
    new Bar();

    echo 'Done';

?>
--EXPECT--
Done
