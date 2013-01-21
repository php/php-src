--TEST--
Isset/unset inherit get/set visibility
--FILE--
<?php

abstract class Test {
    public $foo {
        protected get;
        private set;
    }
    public $bar {
        abstract get;
        final set;
    }
}

echo new ReflectionProperty('Test', 'foo');
echo new ReflectionProperty('Test', 'bar');

?>
--EXPECTF--
Accessor [ public $foo ] {
    Method [ <user> protected method $foo->get ] {
      @@ %s %d - %d
    }
    Method [ <user> private method $foo->set ] {
      @@ %s %d - %d

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
    Method [ <user> protected method $foo->isset ] {
      @@ %s %d - %d
    }
    Method [ <user> private method $foo->unset ] {
      @@ %s %d - %d
    }
}
 ]
Accessor [ public $bar ] {
    Method [ <user> abstract public method $bar->get ] {
      @@ %s %d - %d
    }
    Method [ <user> final public method $bar->set ] {
      @@ %s %d - %d

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
    Method [ <user> abstract public method $bar->isset ] {
      @@ %s %d - %d
    }
    Method [ <user> final public method $bar->unset ] {
      @@ %s %d - %d
    }
}
 ]
