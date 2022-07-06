--TEST--
addcslashes(); function test with warning
--CREDITS--
 marcosptf - <marcosptf@yahoo.com.br>
#phptestfest PHPSP on Google - Sao Paulo - Brazil - 2014-06-05
--FILE--
<?php
echo addcslashes("zoo['.']","z..A");
?>
--EXPECTF--
Warning: addcslashes(): Invalid '..'-range, '..'-range needs to be incrementing in %s on line %d
\zoo['\.']
