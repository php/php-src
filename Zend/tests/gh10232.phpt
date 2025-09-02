--TEST--
GH-10232 (Weird behaviour when a file is autoloaded in assignment of a constant)
--FILE--
<?php

set_include_path('gh10232-nonexistent') or exit(1);
chdir(__DIR__) or exit(1);

spl_autoload_register(function () {
    trigger_error(__LINE__);
    $ex = new Exception();
    echo 'Exception on line ', $ex->getLine(), "\n";
    require_once __DIR__ . '/gh10232/constant_def.inc';
}, true);


class ConstantRef
{
    public const VALUE = ConstantDef::VALUE;
}

ConstantRef::VALUE;

?>
--EXPECTF--
Notice: 7 in %sgh10232.php on line 7
Exception on line 8

Notice: constant_def.inc in %sconstant_def.inc on line 3
Exception in constant_def.inc on line 4

Notice: required.inc in %srequired.inc on line 3
Exception in required.inc on line 4
