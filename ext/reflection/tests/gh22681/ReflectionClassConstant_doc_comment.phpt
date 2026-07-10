--TEST--
GH-22681: null bytes in doc comment truncate ReflectionClassConstant::__toString()
--FILE--
<?php

eval(<<<END
class Demo {
    /** F\0oo */
    public const DEMO = true;
}
END
);

$r = new ReflectionClassConstant(Demo::class, 'DEMO');
echo $r;
var_dump( $r->getDocComment() );

echo new ReflectionClass(Demo::class);

?>
--EXPECTF--
/** F
Constant [ public bool DEMO ] { 1 }
string(11) "/** F%0oo */"
Class [ <user> class Demo ] {
  @@ %s(%d) : eval()'d code %d-%d

  - Constants [1] {
    /** F
    Constant [ public bool DEMO ] { 1 }
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
