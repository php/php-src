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
PHP Warning:  Directive 'magic_quotes_gpc' is deprecated in PHP 5.3 and greater in Unknown on line 0
