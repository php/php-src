--TEST--
SPL: iterator_count() throws if the Iterator throws
--FILE--
<?php

function generator() {
  yield 1;
  throw new Exception('Iterator failed');
}

var_dump(iterator_count(generator()));

?>
--EXPECTF--
Fatal error: Uncaught Exception: Iterator failed in %siterator_count_exception.php:5
Stack trace:
#0 [internal function]: generator()
#1 %siterator_count_exception.php(8): iterator_count(Object(Generator))
#2 {main}
  thrown in %siterator_count_exception.php on line 5
