--TEST--
Ensure __autoload is called twice if unserialize_callback_func is defined.
--FILE--
<?php
spl_autoload_register(function ($name) {
    echo "in autoload($name)\n";
});

ini_set('unserialize_callback_func','check');

function check($name) {
    echo "in check($name)\n";
}

$o = unserialize('O:3:"FOO":0:{}');

var_dump($o);

echo "Done";
?>
--EXPECTF--
in autoload(FOO)
in check(FOO)
in autoload(FOO)

Warning: unserialize(): Function check() hasn't defined the class it was called for in %s on line %d
object(__PHP_Incomplete_Class)#%d (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(3) "FOO"
}
Done
