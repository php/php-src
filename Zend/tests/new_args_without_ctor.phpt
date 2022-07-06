--TEST--
Argument of new on class without constructor are evaluated
--FILE--
<?php

new stdClass(print 'a', print 'b');

?>
--EXPECT--
ab
