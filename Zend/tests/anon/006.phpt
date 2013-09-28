--TEST--
testing anons inside namespaces
--FILE--
<?php
namespace lone {
    class Outer {
        
        public function __construct() {
             var_dump(
                new class{});
        }
    }
    
    new Outer();
}
?>
--EXPECT--	
object(Outer$$1)#2 (0) {
}



