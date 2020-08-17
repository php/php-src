--TEST--
ReflectionExtension::__construct()
--CREDITS--
Gerrit "Remi" te Sligte <remi@wolerized.com>
Leon Luijkx <leon@phpgg.nl>
--FILE--
<?php
try {
    $obj = new ReflectionExtension();
} catch (TypeError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
}

try {
    $obj = new ReflectionExtension('foo', 'bar');
} catch (TypeError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
}

try {
    $obj = new ReflectionExtension([]);
} catch (TypeError $re) {
    echo "Ok - ".$re->getMessage().PHP_EOL;
}


?>
--EXPECT--
Ok - ReflectionExtension::__construct(): Exactly 1 argument is expected, 0 given
Ok - ReflectionExtension::__construct(): Exactly 1 argument is expected, 2 given
Ok - ReflectionExtension::__construct(): Argument #1 ($name) must be of type string, array given
