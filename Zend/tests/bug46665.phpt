--TEST--
Bug #46665 (Triggering autoload with a variable classname causes truncated autoload param)
--FILE--
<?php

$baz = '\\Foo\\Bar\\Baz';
new $baz();
function __autoload($class) {
    var_dump($class);
    require __DIR__ .'/bug46665_autoload.inc';
}

?>
--EXPECTF--
%string|unicode%(11) "Foo\Bar\Baz"
