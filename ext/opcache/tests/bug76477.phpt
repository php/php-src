--TEST--
Bug #76477 (Opcache causes empty return value)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
testString();
function testString()
{
    $token = "ABC";
    $lengthBytes = strlenb($token);
    var_dump($lengthBytes == 0);
}

function strlenb() { return call_user_func_array("strlen", func_get_args()); }
?>
--EXPECT--
bool(false)
