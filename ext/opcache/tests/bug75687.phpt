--TEST--
Bug #75687 (var 8 (TMP) has array key type but not value type)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php

function x($y)
{
    if (is_array($y)) {
        $z = is_array($y) ? array() : array($y);
    }
}
?>
okey
--EXPECT--
okey
