--TEST--
testing anons in functions outside of classes in namespaces
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
?>
--EXPECT--	
object(my_factory$$1)#2 (0) {
}



