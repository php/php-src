--TEST--
Bug #74152 (if statement says true to a null variable)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php

$foo = 'foo';

$bar = null;

switch ($foo) {
default:
case 'foo':
    if ($bar) {
        echo 'true';
    } else {
        echo 'false';
    }
}
?>
--EXPECT--
false
