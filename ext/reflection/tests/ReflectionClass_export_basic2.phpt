--TEST--
ReflectionClass::__toString() - ensure inherited private props are hidden.
--FILE--
<?php
Class c {
	private $a;
	static private $b;
}

class d extends c {}

echo new ReflectionClass("c"), "\n";
echo new ReflectionClass("d"), "\n";
?>
--EXPECTF--
Class [ <user> class c ] {
  @@ %s 2-5

  - Constants [0] {
  }

  - Static properties [1] {
    Property [ private static $b ]
  }

  - Static methods [0] {
  }

  - Properties [1] {
    Property [ <default> private $a ]
  }

  - Methods [0] {
  }
}

Class [ <user> class d extends c ] {
  @@ %s 7-7

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
