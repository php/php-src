--TEST--
ReflectionClass object default property - used to say "..."
--FILE--
<?php

class C {
	public stdClass $a = FOO;
}
$reflector = new ReflectionClass(C::class);
$c = $reflector->newLazyGhost(function () {});

define('FOO', new stdClass);

$c->a;

var_dump( (string)$reflector );

?>
--EXPECTF--
string(%d) "Class [ <user> class C ] {
  @@ %sReflectionClass-dots.php %d-%d

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [1] {
    Property [ public stdClass $a = object(stdClass) ]
  }

  - Methods [0] {
  }
}
"
