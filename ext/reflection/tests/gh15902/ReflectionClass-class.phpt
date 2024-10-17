--TEST--
ReflectionClass object default property - used to say "__CLASS__"
--FILE--
<?php

class C {
	public stdClass $a = FOO;
}
$reflector = new ReflectionClass(C::class);

define('FOO', new stdClass);
new C;

var_dump( (string)$reflector );

?>
--EXPECTF--
string(%d) "Class [ <user> class C ] {
  @@ %sReflectionClass-class.php %d-%d

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
