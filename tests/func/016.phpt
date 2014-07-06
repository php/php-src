--TEST--
bin2hex(); function test
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
#phptestfest PHPSP on Google - Sao Paulo - Brazil - 2014-06-05
--FILE--
<?php
echo bin2hex("123456");
?>
--EXPECT--
313233343536
