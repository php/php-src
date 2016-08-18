--TEST--
testing anon classes in functions outside of classes in namespaces
--FILE--
<?php
namespace lone {
    function my_factory() {
        return new class{};
    }

    class Outer {

        public function __construct() {
             var_dump(
                my_factory());
        }
    }

    new Outer();
}
--EXPECTF--
object(class@%s)#2 (0) {
}

