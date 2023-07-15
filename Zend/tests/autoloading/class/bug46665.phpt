--TEST--
Bug #46665 (Triggering autoload with a variable classname causes truncated autoload param)
--FILE--
<?php

autoload_register_class(function ($class) {
    var_dump($class);
    require __DIR__ .'/bug46665_autoload.inc';
});

$baz = '\\Foo\\Bar\\Baz';
new $baz();

?>
--EXPECT--
string(11) "Foo\Bar\Baz"
