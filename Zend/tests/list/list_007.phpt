--TEST--
Using lambda with list()
--FILE--
<?php

list($x, $y) = function() { };

var_dump($x, $y);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use object of type Closure as array in %slist_007.php:3
Stack trace:
#0 {main}
  thrown in %slist_007.php on line 3
