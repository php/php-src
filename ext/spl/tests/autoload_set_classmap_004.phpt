--TEST--
autoload_set_classmap Map can only be initialized once
--FILE--
<?php

try {
    autoload_set_classmap(['foo' => '/Foo']);
    autoload_set_classmap(['foo' => '/Foo']);
}
catch (\Throwable $ex) {
    echo get_class($ex) . ' - ' . $ex->getMessage();
}

?>
--EXPECT--
Error - Classmap Autoloader can only be initialized once per request.
