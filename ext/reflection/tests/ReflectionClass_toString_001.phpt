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
    Constant [ integer IS_IMPLICIT_ABSTRACT ] { 16 }
    Constant [ integer IS_EXPLICIT_ABSTRACT ] { 32 }
    Constant [ integer IS_FINAL ] { 64 }
  }

  - Static properties [0] {
  }

  - Static methods [1] {
    Method [ <internal:Reflection> static public method export ] {

      - Parameters [2] {
        Parameter #0 [ <required> $argument ]
        Parameter #1 [ <optional> $return ]
      }
    }
  }

  - Properties [1] {
    Property [ <default> public $name ]
  }

  - Methods [40] {
    Method [ <internal:Reflection> final private method __clone ] {
    }

    Method [ <internal:Reflection, ctor> public method __construct ] {

      - Parameters [1] {
        Parameter #0 [ <required> $argument ]
      }
    }

    Method [ <internal:Reflection> public method __toString ] {
    }

    Method [ <internal:Reflection> public method getName ] {
    }

    Method [ <internal:Reflection> public method isInternal ] {
    }

    Method [ <internal:Reflection> public method isUserDefined ] {
    }

    Method [ <internal:Reflection> public method isInstantiable ] {
    }

    Method [ <internal:Reflection> public method getFileName ] {
    }

    Method [ <internal:Reflection> public method getStartLine ] {
    }

    Method [ <internal:Reflection> public method getEndLine ] {
    }

    Method [ <internal:Reflection> public method getDocComment ] {
    }

    Method [ <internal:Reflection> public method getConstructor ] {
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
    }

    Method [ <internal:Reflection> public method getConstant ] {

      - Parameters [1] {
        Parameter #0 [ <required> $name ]
      }
    }

    Method [ <internal:Reflection> public method getInterfaces ] {
    }

    Method [ <internal:Reflection> public method getInterfaceNames ] {
    }

    Method [ <internal:Reflection> public method isInterface ] {
    }

    Method [ <internal:Reflection> public method isAbstract ] {
    }

    Method [ <internal:Reflection> public method isFinal ] {
    }

    Method [ <internal:Reflection> public method getModifiers ] {
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

    Method [ <internal:Reflection> public method newInstanceArgs ] {

      - Parameters [1] {
        Parameter #0 [ <optional> array $args ]
      }
    }

    Method [ <internal:Reflection> public method getParentClass ] {
    }

    Method [ <internal:Reflection> public method isSubclassOf ] {

      - Parameters [1] {
        Parameter #0 [ <required> $class ]
      }
    }

    Method [ <internal:Reflection> public method getStaticProperties ] {
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
    }

    Method [ <internal:Reflection> public method isIterateable ] {
    }

    Method [ <internal:Reflection> public method implementsInterface ] {

      - Parameters [1] {
        Parameter #0 [ <required> $interface ]
      }
    }

    Method [ <internal:Reflection> public method getExtension ] {
    }

    Method [ <internal:Reflection> public method getExtensionName ] {
    }
  }
}
