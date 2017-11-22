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
--EXPECTF--
Class [ <internal:Reflection> class ReflectionClass implements Reflector ] {

  - Constants [3] {
    Constant [ public integer IS_IMPLICIT_ABSTRACT ] { 16 }
    Constant [ public integer IS_EXPLICIT_ABSTRACT ] { 32 }
    Constant [ public integer IS_FINAL ] { 4 }
  }

  - Static properties [0] {
  }

  - Static methods [1] {
    Method [ <internal:Reflection, prototype Reflector> static public method export ] {

      - Parameters [2] {
        Parameter #0 [ <required> $argument ]
        Parameter #1 [ <optional> $return ]
      }
    }
  }

  - Properties [1] {
    Property [ <default> public $name ]
  }

  - Methods [53] {
    Method [ <internal:Reflection> final private method __clone ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection, ctor> public method __construct ] {

      - Parameters [1] {
        Parameter #0 [ <required> $argument ]
      }
    }

    Method [ <internal:Reflection, prototype Reflector> public method __toString ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getName ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method isInternal ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method isUserDefined ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method isAnonymous ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method isInstantiable ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method isCloneable ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getFileName ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getStartLine ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getEndLine ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getDocComment ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getConstructor ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method hasMethod ] {

      - Parameters [1] {
        Parameter #0 [ <required> $name ]
      }
    }

    Method [ <internal:Reflection> public method getMethod ] {

      - Parameters [1] {
        Parameter #0 [ <required> $name ]
      }
    }

    Method [ <internal:Reflection> public method getMethods ] {

      - Parameters [1] {
        Parameter #0 [ <optional> $filter ]
      }
    }

    Method [ <internal:Reflection> public method hasProperty ] {

      - Parameters [1] {
        Parameter #0 [ <required> $name ]
      }
    }

    Method [ <internal:Reflection> public method getProperty ] {

      - Parameters [1] {
        Parameter #0 [ <required> $name ]
      }
    }

    Method [ <internal:Reflection> public method getProperties ] {

      - Parameters [1] {
        Parameter #0 [ <optional> $filter ]
      }
    }

    Method [ <internal:Reflection> public method hasConstant ] {

      - Parameters [1] {
        Parameter #0 [ <required> $name ]
      }
    }

    Method [ <internal:Reflection> public method getConstants ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getReflectionConstants ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getConstant ] {

      - Parameters [1] {
        Parameter #0 [ <required> $name ]
      }
    }

    Method [ <internal:Reflection> public method getReflectionConstant ] {

      - Parameters [1] {
        Parameter #0 [ <required> $name ]
      }
    }

    Method [ <internal:Reflection> public method getInterfaces ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getInterfaceNames ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method isInterface ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getTraits ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getTraitNames ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getTraitAliases ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method isTrait ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method isAbstract ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method isFinal ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getModifiers ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method isInstance ] {

      - Parameters [1] {
        Parameter #0 [ <required> $object ]
      }
    }

    Method [ <internal:Reflection> public method newInstance ] {

      - Parameters [1] {
        Parameter #0 [ <required> $args ]
      }
    }

    Method [ <internal:Reflection> public method newInstanceWithoutConstructor ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method newInstanceArgs ] {

      - Parameters [1] {
        Parameter #0 [ <optional> array $args ]
      }
    }

    Method [ <internal:Reflection> public method getParentClass ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method isSubclassOf ] {

      - Parameters [1] {
        Parameter #0 [ <required> $class ]
      }
    }

    Method [ <internal:Reflection> public method getStaticProperties ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getStaticPropertyValue ] {

      - Parameters [2] {
        Parameter #0 [ <required> $name ]
        Parameter #1 [ <optional> $default ]
      }
    }

    Method [ <internal:Reflection> public method setStaticPropertyValue ] {

      - Parameters [2] {
        Parameter #0 [ <required> $name ]
        Parameter #1 [ <required> $value ]
      }
    }

    Method [ <internal:Reflection> public method getDefaultProperties ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method isIterable ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method isIterateable ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method implementsInterface ] {

      - Parameters [1] {
        Parameter #0 [ <required> $interface ]
      }
    }

    Method [ <internal:Reflection> public method getExtension ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getExtensionName ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method inNamespace ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getNamespaceName ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Reflection> public method getShortName ] {

      - Parameters [0] {
      }
    }
  }
}
