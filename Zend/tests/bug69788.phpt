--TEST--
Bug #69788: Malformed script causes Uncaught Error in php-cgi, valgrind SIGILL
--FILE--
<?php [t.[]]; ?>
--EXPECTF--
Warning: Use of undefined constant t - assumed 't' (this will throw an Error in a future version of PHP) in %s on line %d

Notice: Array to string conversion in %s on line %d
