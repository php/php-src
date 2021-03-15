--TEST--
autoload_set_classmap Reject non-string values
--FILE--
<?php

    try {
        autoload_set_classmap(['Foo' => 123]);
        new Foo();
    }
    catch (\Throwable $ex) {
        echo get_class($ex) . ' - ' . $ex->getMessage();
    }

?>
--EXPECT--
Error - Error during autoloading from classmap. Entry "Foo" expected a string value, "int" given.
