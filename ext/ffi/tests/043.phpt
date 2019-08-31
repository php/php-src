--TEST--
FFI 043: Type redeclaration
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
FFI::cdef("
typedef int a;
typedef unsigned int a;
");
?>
--EXPECTF--
Fatal error: Uncaught FFI\ParserException: redeclaration of 'a' at line 3 in %s043.php:2
Stack trace:
#0 %s043.php(2): FFI::cdef('%s')
#1 {main}
  thrown in %s043.php on line 2
