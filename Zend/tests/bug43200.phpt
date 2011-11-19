--TEST--
Bug #43200 (Interface implementation / inheritence not possible in abstract classes)
--FILE--
<?php

interface a {
	function foo();
	function bar();
}
interface b {
	function foo();
}

abstract class c {
	function bar() { }
}

class x extends c implements a, b {
	function foo() { }
}

ReflectionClass::export('x');

?>
--EXPECTF--
Class [ <user> class x extends c implements a, b ] {
  @@ %s 15-17

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [2] {
    Method [ <user, prototype b> public method foo ] {
      @@ %s 16 - 16
    }

    Method [ <user, inherits c, prototype a> public method bar ] {
      @@ %s 12 - 12
    }
  }
}

