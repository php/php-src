--TEST--
Test ReflectionMethod::setFinal().
--FILE--
<?php
class a {
    public final function b() {
        print __METHOD__;
    }
}

$m = new ReflectionMethod('a', 'b');
$m->setFinal(FALSE);

var_dump($m->isFinal());

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
