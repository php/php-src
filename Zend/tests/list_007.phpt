--TEST--
Using lambda with list()
--FILE--
<?php

list($x, $y) = function() { };

var_dump($x, $y);

?>
--EXPECTF--
Fatal error: Cannot use object of type Closure as array in %slist_007.php on line 3
