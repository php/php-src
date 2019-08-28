--TEST--
Argument of new on class without constructor are evaluated
--FILE--
<?php

new StdClass(print 'a', print 'b');

?>
--EXPECT--
ab
