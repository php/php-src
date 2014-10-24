--TEST--
addcslashes(); function test
--CREDITS--
#phptestfest PHPSP on Google - Sao Paulo - Brazil - 2014-06-05
# <marcosptf@yahoo.com.br>
--FILE--
<?php
echo addcslashes("foo[ ]","A..z");
?>
--EXPECT--
\f\o\o\[ \]
