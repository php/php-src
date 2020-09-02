--TEST--
Bug #76446 (zend_variables.c:73: zend_string_destroy: Assertion `!(zval_gc_flags((str)->gc)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function test()
{
    $openmenu = '';
    $openstr2 = "&amp;openmenu={$openmenu}{$addlang}\"";
    return 0;
}

var_dump(test());
?>
--EXPECTF--
Warning: Undefined variable $addlang in %s on line %d
int(0)
