--TEST--
method_exists() on non-existent class, with __autoload().
--FILE--
<?php
spl_autoload_register(function ($name) {
    echo "In autoload($name)\n";
});

var_dump(method_exists('UndefC', 'func'));

echo "Done";
?>
--EXPECT--
In autoload(UndefC)
bool(false)
Done
