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
--EXPECT--
object(lone\@anonymous)#1 (0) {
}
