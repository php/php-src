--TEST--
ReflectionClass object default property - used to say "callable"
--INI--
opcache.enable_cli=0
--FILE--
<?php

class C {
	public stdClass $a = FOO;
}
define('FOO', new stdClass);

new C;

$reflector = new ReflectionClass(C::class);
echo $reflector;
?>
--EXPECTF--
Class [ <user> class C ] {
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
