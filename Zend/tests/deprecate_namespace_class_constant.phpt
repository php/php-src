--TEST--
Using "namespace" as a class constant name is deprecated
--FILE--
<?php

define('namespace', 'global constant value');

class Foo {
    const NAMESPACE = 'class constant value';
}

interface Bar {
    const Namespace = 'interface constant value';
}

enum Baz {
    const nAmEsPaCe = 'enum constant value';
}

trait YooTrait {
    const namespace = 'trait constant value';
}

class Sup {
    use YooTrait;
}

enum Yoo: string {
    case namespace = 'enum case value';
}

class Can {
    public static $namespace = 'property value';

    public static function namespace() {
        return 'method value';
    }
}

echo Foo::NAMESPACE, PHP_EOL;
echo Bar::Namespace, PHP_EOL;
echo Baz::nAmEsPaCe, PHP_EOL;
echo Yoo::namespace->value, PHP_EOL;
echo Sup::namespace, PHP_EOL;
echo Can::$namespace, PHP_EOL;
echo Can::namespace(), PHP_EOL;
echo constant('namespace'), PHP_EOL;

?>
--EXPECTF--
Deprecated: Declaring class constant called 'namespace' is deprecated in %s on line %d

Deprecated: Declaring interface constant called 'namespace' is deprecated in %s on line %d

Deprecated: Declaring enum constant called 'namespace' is deprecated in %s on line %d

Deprecated: Declaring trait constant called 'namespace' is deprecated in %s on line %d

Deprecated: Declaring enum constant called 'namespace' is deprecated in %s on line %d
class constant value
interface constant value
enum constant value
enum case value
trait constant value
property value
method value
global constant value
