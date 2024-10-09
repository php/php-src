--TEST--
ReflectionClass object default property - used to say "callable"
--FILE--
<?php

class C {
	public stdClass $a = FOO;
}
define('FOO', new stdClass);

new C;

$reflector = new ReflectionClass(C::class);
var_dump( (string)$reflector );
?>
--EXPECTF--
string(%d) "Class [ <user> class C ] {
  @@ %sReflectionClass-callable.php %d-%d

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
