--TEST--
autoload_set_classmap Map can only be initialized once
--FILE--
<?php

    try {
        autoload_set_classmap(['Foo' => '/Foo']);
        autoload_set_classmap(['Foo' => '/Foo']);
    }
    catch (\Throwable $ex) {
        echo get_class($ex) . ' - ' . $ex->getMessage();
    }

?>
--EXPECT--
Error - The autoload classmap can only be initialized once per request.