--TEST--
GH-22681: null bytes in doc comment truncate ReflectionClass::__toString()
--FILE--
<?php

eval(<<<END
/** F\0oo */
class Demo {}
END
);

$r = new ReflectionClass(Demo::class);
echo $r;
var_dump( $r->getDocComment() );

?>
--EXPECTF--
/** F
Class [ <user> class Demo ] {
  @@ %s(%d) : eval()'d code %d-%d

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
string(11) "/** F%0oo */"
