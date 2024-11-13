--TEST--
Test that displaying doc comments via reflection is supported by internal symbols
--EXTENSIONS--
zend_test
reflection
--FILE--
<?php

$rc = new ReflectionClass(ZendTestPropertyAttribute::class);
echo $rc->__toString() . "\n";

$rc = new ReflectionClassConstant(_ZendTestInterface::class, "DUMMY");
echo $rc->__toString() . "\n";

$rf = new ReflectionFunction("zend_test_nullable_array_return");
echo $rf->__toString() . "\n";

$rp = new ReflectionProperty(new ZendTestPropertyAttribute(""), "parameter");
echo $rp->__toString() . "\n";

?>
--EXPECT--
/**
     * "Lorem ipsum"
     * @see https://www.php.net
     * @since 8.1
     */
Class [ <internal:zend_test> final class ZendTestPropertyAttribute ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [1] {
    /**
         * "Lorem ipsum"
         * @see https://www.php.net
         * @since 8.4
         */
    Property [ public string $parameter ]
  }

  - Methods [1] {
    Method [ <internal:zend_test, ctor> public method __construct ] {

      - Parameters [1] {
        Parameter #0 [ <required> string $parameter ]
      }
    }
  }
}

/**
         * "Lorem ipsum"
         * @see https://www.php.net
         * @since 8.2
         */
Constant [ public int DUMMY ] { 0 }

/**
     * "Lorem ipsum"
     * @see https://www.php.net
     * @since 8.3
     */
Function [ <internal:zend_test> function zend_test_nullable_array_return ] {

  - Parameters [0] {
  }
  - Return [ ?array ]
}

/**
         * "Lorem ipsum"
         * @see https://www.php.net
         * @since 8.4
         */
Property [ public string $parameter ]
