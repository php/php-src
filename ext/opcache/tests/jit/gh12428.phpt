--TEST--
GH-12428: Assertion with function/tracing JIT
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
function validate($value)
{
    foreach ([0] as $_) {
        $a = &$value->a;
        $value->a ?? null;
    }
}

validate((object) []);
validate((object) []);
validate((object) ['b' => 0]);
?>
DONE
--EXPECT--
DONE
