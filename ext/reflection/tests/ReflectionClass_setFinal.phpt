--TEST--
Test ReflectionClass::setFinal().
--FILE--
<?php
class a {
    public final function b() {
        print __METHOD__;
    }
}

$c = new ReflectionClass('a');
$c->setFinal(FALSE);

var_dump($c->isFinal());

# Not sure if it is by design that the following two lines are required
$m = new ReflectionMethod('a', 'b');
$m->setFinal(FALSE);

if (TRUE) {
    class c extends a {
        public function b() {
            print __METHOD__;
        }
    }
}

$o = new c;
$o->b();
?>
--EXPECT--
bool(false)
c::b
