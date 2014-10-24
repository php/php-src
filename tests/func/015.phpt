--TEST--
addslashes(); function test
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
#phptestfest PHPSP on Google - Sao Paulo - Brazil - 2014-06-05
--FILE--
<?php
echo addslashes("do you like book's O'reilly?");
?>
--EXPECT--
do you like book\'s O\'reilly?
