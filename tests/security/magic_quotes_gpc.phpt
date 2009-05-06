--TEST--
Test if magic_quotes_gpc works as expected
--INI--
magic_quotes_gpc=1
--GET--
a=abc'"%00123
--FILE--
<?php
echo $_GET['a'],"\n";
?>
--EXPECT--
abc\'\"\0123
