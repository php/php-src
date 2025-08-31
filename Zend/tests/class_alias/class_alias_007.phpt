--TEST--
Testing class_alias() using autoload
--FILE--
<?php

spl_autoload_register(function ($a) {
    class foo { }
});

class_alias('foo', 'bar', 1);

var_dump(new foo, new bar);

?>
--EXPECTF--
object(foo)#%d (0) {
}
object(foo)#%d (0) {
}
