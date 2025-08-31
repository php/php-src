--TEST--
Bug #76796: Compile-time evaluation of disabled function in opcache (SCCP) causes segfault
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
disable_functions=strpos
--EXTENSIONS--
opcache
--FILE--
<?php

try {
    var_dump(strpos('foo', 'bar'));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Call to undefined function strpos()
