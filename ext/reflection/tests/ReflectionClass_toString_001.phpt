--TEST--
ReflectionClass::__toString()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
$rc = new ReflectionClass("ReflectionClass");
echo $rc;
?>
--EXPECT--
Class [ <internal:Reflection> class ReflectionClass implements Stringable, Reflector ] {

  - Constants [4] {
    Constant [ public int IS_IMPLICIT_ABSTRACT ] { 16 }
    Constant [ public int IS_EXPLICIT_ABSTRACT ] { 64 }
    Constant [ public int IS_FINAL ] { 32 }
    Constant [ public int IS_READONLY ] { 65536 }
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [1] {
    Property [ public string $name ]
  }

  - Methods [56] {
    Method [ <internal:Reflection> private method __clone ] {

      - Parameters [0] {
      }
      - Return [ void ]
    }

    Method [ <internal:Reflection, ctor> public method __construct ] {

      - Parameters [1] {
        Parameter #0 [ <required> object|string $objectOrClass ]
      }
    }

    Method [ <internal:Reflection, prototype Stringable> public method __toString ] {

      - Parameters [0] {
      }
      - Return [ string ]
    }

    Method [ <internal:Reflection> public method getName ] {

      - Parameters [0] {
      }
      - Tentative return [ string ]
    }

    Method [ <internal:Reflection> public method isInternal ] {

      - Parameters [0] {
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method isUserDefined ] {

      - Parameters [0] {
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method isAnonymous ] {

      - Parameters [0] {
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method isInstantiable ] {

      - Parameters [0] {
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method isCloneable ] {

      - Parameters [0] {
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method getFileName ] {

      - Parameters [0] {
      }
      - Tentative return [ string|false ]
    }

    Method [ <internal:Reflection> public method getStartLine ] {

      - Parameters [0] {
      }
      - Tentative return [ int|false ]
    }

    Method [ <internal:Reflection> public method getEndLine ] {

      - Parameters [0] {
      }
      - Tentative return [ int|false ]
    }

    Method [ <internal:Reflection> public method getDocComment ] {

      - Parameters [0] {
      }
      - Tentative return [ string|false ]
    }

    Method [ <internal:Reflection> public method getConstructor ] {

      - Parameters [0] {
      }
      - Tentative return [ ?ReflectionMethod ]
    }

    Method [ <internal:Reflection> public method hasMethod ] {

      - Parameters [1] {
        Parameter #0 [ <required> string $name ]
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method getMethod ] {

      - Parameters [1] {
        Parameter #0 [ <required> string $name ]
      }
      - Tentative return [ ReflectionMethod ]
    }

    Method [ <internal:Reflection> public method getMethods ] {

      - Parameters [1] {
        Parameter #0 [ <optional> ?int $filter = null ]
      }
      - Tentative return [ array ]
    }

    Method [ <internal:Reflection> public method hasProperty ] {

      - Parameters [1] {
        Parameter #0 [ <required> string $name ]
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method getProperty ] {

      - Parameters [1] {
        Parameter #0 [ <required> string $name ]
      }
      - Tentative return [ ReflectionProperty ]
    }

    Method [ <internal:Reflection> public method getProperties ] {

      - Parameters [1] {
        Parameter #0 [ <optional> ?int $filter = null ]
      }
      - Tentative return [ array ]
    }

    Method [ <internal:Reflection> public method hasConstant ] {

      - Parameters [1] {
        Parameter #0 [ <required> string $name ]
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method getConstants ] {

      - Parameters [1] {
        Parameter #0 [ <optional> ?int $filter = null ]
      }
      - Tentative return [ array ]
    }

    Method [ <internal:Reflection> public method getReflectionConstants ] {

      - Parameters [1] {
        Parameter #0 [ <optional> ?int $filter = null ]
      }
      - Tentative return [ array ]
    }

    Method [ <internal:Reflection> public method getConstant ] {

      - Parameters [1] {
        Parameter #0 [ <required> string $name ]
      }
      - Tentative return [ mixed ]
    }

    Method [ <internal:Reflection> public method getReflectionConstant ] {

      - Parameters [1] {
        Parameter #0 [ <required> string $name ]
      }
      - Tentative return [ ReflectionClassConstant|false ]
    }

    Method [ <internal:Reflection> public method getInterfaces ] {

      - Parameters [0] {
      }
      - Tentative return [ array ]
    }

    Method [ <internal:Reflection> public method getInterfaceNames ] {

      - Parameters [0] {
      }
      - Tentative return [ array ]
    }

    Method [ <internal:Reflection> public method isInterface ] {

      - Parameters [0] {
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method getTraits ] {

      - Parameters [0] {
      }
      - Tentative return [ array ]
    }

    Method [ <internal:Reflection> public method getTraitNames ] {

      - Parameters [0] {
      }
      - Tentative return [ array ]
    }

    Method [ <internal:Reflection> public method getTraitAliases ] {

      - Parameters [0] {
      }
      - Tentative return [ array ]
    }

    Method [ <internal:Reflection> public method isTrait ] {

      - Parameters [0] {
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method isEnum ] {

      - Parameters [0] {
      }
      - Return [ bool ]
    }

    Method [ <internal:Reflection> public method isAbstract ] {

      - Parameters [0] {
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method isFinal ] {

      - Parameters [0] {
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method isReadOnly ] {

      - Parameters [0] {
      }
      - Return [ bool ]
    }

    Method [ <internal:Reflection> public method getModifiers ] {

      - Parameters [0] {
      }
      - Tentative return [ int ]
    }

    Method [ <internal:Reflection> public method isInstance ] {

      - Parameters [1] {
        Parameter #0 [ <required> object $object ]
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method newInstance ] {

      - Parameters [1] {
        Parameter #0 [ <optional> mixed ...$args ]
      }
      - Tentative return [ object ]
    }

    Method [ <internal:Reflection> public method newInstanceWithoutConstructor ] {

      - Parameters [0] {
      }
      - Tentative return [ object ]
    }

    Method [ <internal:Reflection> public method newInstanceArgs ] {

      - Parameters [1] {
        Parameter #0 [ <optional> array $args = [] ]
      }
      - Tentative return [ ?object ]
    }

    Method [ <internal:Reflection> public method getParentClass ] {

      - Parameters [0] {
      }
      - Tentative return [ ReflectionClass|false ]
    }

    Method [ <internal:Reflection> public method isSubclassOf ] {

      - Parameters [1] {
        Parameter #0 [ <required> ReflectionClass|string $class ]
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method getStaticProperties ] {

      - Parameters [0] {
      }
      - Tentative return [ array ]
    }

    Method [ <internal:Reflection> public method getStaticPropertyValue ] {

      - Parameters [2] {
        Parameter #0 [ <required> string $name ]
        Parameter #1 [ <optional> mixed $default = <default> ]
      }
      - Tentative return [ mixed ]
    }

    Method [ <internal:Reflection> public method setStaticPropertyValue ] {

      - Parameters [2] {
        Parameter #0 [ <required> string $name ]
        Parameter #1 [ <required> mixed $value ]
      }
      - Tentative return [ void ]
    }

    Method [ <internal:Reflection> public method getDefaultProperties ] {

      - Parameters [0] {
      }
      - Tentative return [ array ]
    }

    Method [ <internal:Reflection> public method isIterable ] {

      - Parameters [0] {
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method isIterateable ] {

      - Parameters [0] {
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method implementsInterface ] {

      - Parameters [1] {
        Parameter #0 [ <required> ReflectionClass|string $interface ]
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method getExtension ] {

      - Parameters [0] {
      }
      - Tentative return [ ?ReflectionExtension ]
    }

    Method [ <internal:Reflection> public method getExtensionName ] {

      - Parameters [0] {
      }
      - Tentative return [ string|false ]
    }

    Method [ <internal:Reflection> public method inNamespace ] {

      - Parameters [0] {
      }
      - Tentative return [ bool ]
    }

    Method [ <internal:Reflection> public method getNamespaceName ] {

      - Parameters [0] {
      }
      - Tentative return [ string ]
    }

    Method [ <internal:Reflection> public method getShortName ] {

      - Parameters [0] {
      }
      - Tentative return [ string ]
    }

    Method [ <internal:Reflection> public method getAttributes ] {

      - Parameters [2] {
        Parameter #0 [ <optional> ?string $name = null ]
        Parameter #1 [ <optional> int $flags = 0 ]
      }
      - Return [ array ]
    }
  }
}
