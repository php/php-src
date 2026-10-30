--TEST--
Don't segfault when zend.script_encoding=pass
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
zend.script_encoding=pass
internal_encoding=UTF-8
--FILE--
<?php
	print "Done!\n";
?>
--EXPECT--
Warning: PHP Startup: INI setting contains invalid encoding "pass" in Unknown on line 0
Done!
