--TEST--
Bug #38942 (Double old-style-ctor inheritance)
--FILE--
<?php
class foo {
    public function foo() {}
}

class bar extends foo {
}
ReflectionClass::export("bar");
?>
--EXPECTF--
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
