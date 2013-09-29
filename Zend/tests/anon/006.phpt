--TEST--
testing anons inside namespaces
--FILE--
<?php
namespace lone {
   $hello = new class{} ;
}

namespace {
    var_dump ($hello);
}
?>
--EXPECT--	
object(Class$$1)#1 (0) {
}



