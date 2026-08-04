--TEST--
GH-22681: null bytes in doc comment truncate ReflectionFunctionAbstract::__toString()
--FILE--
<?php

eval(<<<END
/** F\0oo */
function demo() {}

class Demo {
    /** B\0ar */
    public function demo() {}
}
END
);

$r = new ReflectionFunction('demo');
echo $r;
var_dump( $r->getDocComment() );

$r = new ReflectionMethod(Demo::class, 'demo');
echo $r;
var_dump( $r->getDocComment() );

?>
--EXPECTF--
/** F%0oo */
Function [ <user> function demo ] {
  @@ %s(%d) : eval()'d code %d - %d
}
string(11) "/** F%0oo */"
/** B%0ar */
Method [ <user> public method demo ] {
  @@ %s(%d) : eval()'d code %d - %d
}
string(11) "/** B%0ar */"
