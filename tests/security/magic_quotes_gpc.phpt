--TEST--
Test if magic_quotes_gpc works as expected
--INI--
display_errors=0
magic_quotes_gpc=1
--GET--
a=abc'"%00123
--FILE--
<?php
echo $_GET['a'],"\n";
?>
--EXPECT--
abc\'\"\0123
