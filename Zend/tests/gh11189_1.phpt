--TEST--
GH-11189: Exceeding memory limit in zend_hash_do_resize leaves the array in an invalid state (not packed array)
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip ZMM is disabled");
?>
--INI--
memory_limit=2M
--FILE--
<?php

ob_start(function() {
    global $a;
    for ($i = count($a); $i > 0; --$i) {
        $a[] = 2;
    }
    fwrite(STDOUT, "Success");
});

$a = ["not packed" => 1];
// trigger OOM in a resize operation
while (1) {
    $a[] = 1;
}

?>
--EXPECTF--
Success
Fatal error: Allowed memory size of %s bytes exhausted%s(tried to allocate %s bytes) in %s on line %d
