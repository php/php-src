--TEST--
class name as scalar from ::class keyword error using parent in method signature
--FILE--
<?php

namespace Foo\Bar {
    class One {}
    class Two extends One {
        public function baz($x = parent::class) {
            var_dump($x);
        }
    }
    (new Two)->baz();
}
?>
--EXPECT--
string(11) "Foo\Bar\One"
