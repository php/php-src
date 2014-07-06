--TEST--
addcslashes() function test
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
#phptestfest PHPSP on Google - Sao Paulo - Brazil - 2014-06-05
--FILE--
<?php error_reporting(0);
echo addcslashes("zoo['.']","z..A");
?>
--EXPECT--
\zoo['\.']
