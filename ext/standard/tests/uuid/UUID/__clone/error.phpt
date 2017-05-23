--TEST--
UUID::__clone invocation leads to Error
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$m = new ReflectionMethod(UUID::class, '__clone');
$m->setAccessible(true);

try {
	$m->invoke(UUID::Nil());
}
catch (Error $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Cannot clone immutable UUID object
