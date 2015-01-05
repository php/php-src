--TEST--
Bug #48023 (spl_autoload_register didn't addref closures)
--FILE--
<?php
spl_autoload_register(function(){});

new Foo;

?>
===DONE===
--EXPECTF--
Fatal error: Class 'Foo' not found in %s on line %d
