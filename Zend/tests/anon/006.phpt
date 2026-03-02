--TEST--
testing anon classes inside namespaces
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
object(class@%s)#1 (0) {
}
