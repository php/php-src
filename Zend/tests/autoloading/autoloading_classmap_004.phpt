--TEST--
Tests autoloading from a classmap where the keys have already been lowercased
--FILE--
<?php

    autoload_classmap(['bar' => __DIR__ . '/Bar.php'], true);

    new Bar();

    echo 'Done';

?>
--EXPECT--
Done
