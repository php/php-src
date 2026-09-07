--TEST--
ReflectionConstant::inNamespace()
--FILE--
<?php

namespace Foo\Bar {
    const NAMESPACED_CONST = 'in namespace';
}

namespace {
    const GLOBAL_CONST = 'global';

    $rc1 = new ReflectionConstant('GLOBAL_CONST');
    var_dump($rc1->inNamespace());
    var_dump($rc1->getNamespaceName());
    var_dump($rc1->getShortName());

    $rc2 = new ReflectionConstant('Foo\Bar\NAMESPACED_CONST');
    var_dump($rc2->inNamespace());
    var_dump($rc2->getNamespaceName());
    var_dump($rc2->getShortName());

    $rc3 = new ReflectionConstant('E_ERROR');
    var_dump($rc3->inNamespace());
    var_dump($rc3->getNamespaceName());
    var_dump($rc3->getShortName());
}

?>
--EXPECT--
bool(false)
string(0) ""
string(12) "GLOBAL_CONST"
bool(true)
string(7) "Foo\Bar"
string(16) "NAMESPACED_CONST"
bool(false)
string(0) ""
string(7) "E_ERROR"
