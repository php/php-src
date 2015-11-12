--TEST--
Bug #70895 null ptr deref and segfault with crafted calable
--FILE--
<?php

array_map("%n", 0);
array_map("%n %i", 0);
array_map("%n %i aoeu %f aoeu %p", 0);
?>
--EXPECTREGEX--
Warning: array_map\(\) expects parameter 1 to be a valid callback, function '%n' not found or invalid function name in .+

Warning: array_map\(\) expects parameter 1 to be a valid callback, function '%n %i' not found or invalid function name in .+

Warning: array_map\(\) expects parameter 1 to be a valid callback, function '%n %i aoeu %f aoeu %p' not found or invalid function name in .+bug70895.php on line \d+
