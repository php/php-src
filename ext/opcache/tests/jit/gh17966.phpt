--TEST--
GH-17966 (Symfony JIT 1205 assertion failure)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1205
--FILE--
<?php
function test($value, bool $test)
{
    $value = (float) $value;

    if ($test) {
        return $value * 2;
    }

    return $value;
}

var_dump(test(1.25, true));
var_dump(test(1.25, false));
?>
--EXPECT--
float(2.5)
float(1.25)
