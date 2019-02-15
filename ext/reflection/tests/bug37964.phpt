--TEST--
Reflection Bug #37964 (Reflection shows private methods of parent class)
--FILE--
<?php

abstract class foobar {
	private function test2() {
	}
}
class foo extends foobar {
	private $foo = 1;
	private function test() {
	}
	protected function test3() {
	}
}
class bar extends foo {
	private function foobar() {
	}
}

Reflection::export(new ReflectionClass(new bar));

?>
--EXPECTF--
Class [ <user> class bar extends foo ] {
  @@ %s %s

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [2] {
    Method [ <user> private method foobar ] {
      @@ %s %d - %d
    }

    Method [ <user, inherits foo> protected method test3 ] {
      @@ %s %d - %d
    }
  }
}
