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
--EXPECTF--
object(%s$$1)#1 (0) {
}



