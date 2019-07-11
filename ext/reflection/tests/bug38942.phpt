--TEST--
Bug #38942 (Double old-style-ctor inheritance)
--FILE--
<?php
class foo {
    public function foo() {}
}

class bar extends foo {
}
echo new ReflectionClass("bar");
?>
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; foo has a deprecated constructor in %s on line %d
Class [ <user> class bar extends foo ] {
  @@ %sbug38942.php 6-7

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [1] {
    Method [ <user, inherits foo, ctor> public method foo ] {
      @@ %sbug38942.php 3 - 3
    }
  }
}
