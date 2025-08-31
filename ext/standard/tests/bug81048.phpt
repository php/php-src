--TEST--
Bug #81048 (phpinfo(INFO_VARIABLES) "Array to string conversion")
--FILE--
<?php

$_ENV = [];
$_SERVER = ['foo' => ['bar' => ['baz' => 'qux']]];

array_walk_recursive($_SERVER, function($value, $key) {
    // NOP
});

phpinfo(INFO_VARIABLES);
?>
--EXPECT--
phpinfo()

PHP Variables

Variable => Value
$_SERVER['foo'] => Array
(
    [bar] => Array
        (
            [baz] => qux
        )

)
