--TEST--
Testing nested list() with empty array
--FILE--
<?php

list($a, list($b, list(list($d)))) = array();

?>
--EXPECTF--
Notice: Undefined offset: 0 in %s on line %d

Notice: Undefined offset: 1 in %s on line %d

Warning: Variable of type null does not accept array offsets in %s on line %d

Warning: Variable of type null does not accept array offsets in %s on line %d

Warning: Variable of type null does not accept array offsets in %s on line %d

Warning: Variable of type null does not accept array offsets in %s on line %d
