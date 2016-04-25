--TEST--
basic union implicit nullability allowed on params
--FILE--
<?php
$foo = function(Foo|Bar $foo = null) {
	var_dump($foo);
};
$foo(null);
?>
--EXPECT--
NULL
