--TEST--
GHSA-8mjj-2r7p-4hg2
--INI--
fatal_error_backtraces=1
zend.exception_ignore_args=0
memory_limit=4M
opcache.jit=disable
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip requires ZendMM");
?>
--FILE--
<?php

class C {
    static function __callStatic($name, $args)
    {
    }
}

$a = str_repeat('a', 1*1024*1024);

// Soft OOM during CONCAT will be handled by DO_FCALL
C::test(1, 2, 3, 4, $a.$a);

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line 12
Stack trace:
#0 %s(12): C::test(1, 2, 3, 4, 'aaaaaaaaaaaaaaa...')
#1 {main}
