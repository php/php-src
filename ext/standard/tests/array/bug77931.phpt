--TEST--
Bug #77931: Warning for array_map mentions wrong type
--FILE--
<?php

array_map('trim', array(), 1);
array_map('trim', array(), array(), true);
array_map('trim', array(), array(), array(), null);

?>
--EXPECTF--
Warning: array_map(): Expected parameter 3 to be an array, int given in %s on line %d

Warning: array_map(): Expected parameter 4 to be an array, bool given in %s on line %d

Warning: array_map(): Expected parameter 5 to be an array, null given in %s on line %d
