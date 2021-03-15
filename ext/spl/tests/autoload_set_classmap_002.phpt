--TEST--
autoload_set_classmap Reject non-string values
--FILE--
<?php

    try {
        autoload_set_classmap(['Foo' => 123]);
    }
    catch (\Throwable $ex) {
        echo get_class($ex) . ' - ' . $ex->getMessage();
    }

?>
--EXPECT--
ValueError - autoload_set_classmap(): Argument #1 ($mapping) Classmap entry for "Foo" expected string value, found int.