--TEST--
Bug #69788: Malformed script causes Uncaught Error in php-cgi, valgrind SIGILL
--FILE--
<?php [t.[]]; ?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "t" in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
