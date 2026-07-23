--TEST--
GH-22681: null bytes in doc comment truncate ReflectionProperty::__toString()
--FILE--
<?php

eval(<<<END
class Demo {
    /** F\0oo */
    public \$prop;
}
END
);

$r = new ReflectionProperty(Demo::class, 'prop');
echo $r;
var_dump( $r->getDocComment() );

echo new ReflectionClass(Demo::class);

?>
--EXPECTF--
/** F%0oo */
Property [ public $prop = NULL ]
string(11) "/** F%0oo */"
Class [ <user> class Demo ] {
  @@ %s(%d) : eval()'d code %d-%d

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [1] {
    /** F%0oo */
    Property [ public $prop = NULL ]
  }

  - Methods [0] {
  }
}
