--TEST--
autoload_set_classmap Reject non-string values
--FILE--
<?php

try {
    autoload_set_classmap(['foo' => 123]);
    new Foo();
}
catch (\Throwable $ex) {
    echo get_class($ex) . ' - ' . $ex->getMessage();
}

?>
--EXPECT--
TypeError - Error during autoloading from classmap. Entry "foo" expected a string value, int given
