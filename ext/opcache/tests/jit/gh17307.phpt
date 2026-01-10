--TEST--
GH-17307 (Internal closure causes JIT failure)
--EXTENSIONS--
opcache
simplexml
bcmath
--INI--
opcache.jit=1254
opcache.jit_hot_func=1
opcache.jit_buffer_size=32M
--FILE--
<?php

$simple = new SimpleXMLElement("<root><a/><b/></root>");

function run_loop($firstTerms, $closure) {
    foreach ($firstTerms as $firstTerm) {
        \debug_zval_dump($firstTerm);
        $closure($firstTerm, "10");
    }
}

run_loop($simple, bcadd(...));
echo "Done\n";

?>
--EXPECTF--
object(SimpleXMLElement)#%d (0) refcount(3){
}
object(SimpleXMLElement)#%d (0) refcount(3){
}
Done
