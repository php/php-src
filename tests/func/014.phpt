--TEST--
addcslashes() function test
--CREDITS--
#phptestfest PHPSP on Google - Sao Paulo - Brazil - 2014-06-05
--FILE--
<?php error_reporting(0);
echo addcslashes("foo[ ]","A..z");
?>
--EXPECT--
\f\o\o\[ \]
