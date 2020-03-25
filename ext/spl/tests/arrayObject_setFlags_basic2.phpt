--TEST--
SPL: Ensure access to non-visible properties falls back to dimension access with ArrayObject::ARRAY_AS_PROPS.
--FILE--
<?php
class C extends ArrayObject {
    private $x = 'secret';

    static function go($c) {
      var_dump($c->x);
    }
}

$c = new C(array('x'=>'public'));

$c->setFlags(ArrayObject::ARRAY_AS_PROPS);
C::go($c);
var_dump($c->x);


$c->setFlags(0);
C::go($c);
var_dump($c->x);
?>
--EXPECTF--
string(6) "secret"
string(6) "public"
string(6) "secret"

Fatal error: Uncaught Error: Cannot access private property C::$x in %s:19
Stack trace:
#0 {main}
  thrown in %s on line 19
