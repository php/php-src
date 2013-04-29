--TEST--
ReflectionClass and Traits
--FILE--
<?php

abstract class foo {
}

trait bar {
	
}

reflectionclass::export('foo');
reflectionclass::export('bar');

?>
--EXPECTF--
Class [ <user> abstract class foo ] {
  @@ %s 3-4

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [0] {
  }
}

Trait [ <user> trait bar ] {
  @@ %s 6-8

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [0] {
  }
}
