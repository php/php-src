--TEST--
GH-12895 (Incorrect ReflectionClass::getNamespaceName for anonymous class)
--FILE--
<?php

namespace Foo {
    interface InterfaceFoo {}
}

namespace Bar {
    new class {};
}

namespace Bar\Baz {
    new class implements \Foo\InterfaceFoo {};
}

namespace {
    new class implements \Foo\InterfaceFoo {};

    foreach (get_declared_classes() as $class) {
        if (!str_contains($class, '@anonymous')) {
            continue;
        }
        $rc = new ReflectionClass($class);
        echo 'Class: ', $class, PHP_EOL;
        echo 'getNamespaceName: ', $rc->getNamespaceName(), PHP_EOL;
        echo 'getShortName: ', $rc->getShortName(), PHP_EOL;
        echo 'inNamespace: ', $rc->inNamespace() ? "true" : "false", PHP_EOL;
        echo "---\n";
    }
}

?>
--EXPECTF--
Class: Bar\@anonymous%s
getNamespaceName: Bar
getShortName: @anonymous%s
inNamespace: true
---
Class: Bar\Baz\@anonymous%s
getNamespaceName: Bar\Baz
getShortName: @anonymous%s
inNamespace: true
---
Class: class@anonymous%s
getNamespaceName: 
getShortName: class@anonymous%s
inNamespace: false
---
