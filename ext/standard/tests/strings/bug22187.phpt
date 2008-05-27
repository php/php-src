--TEST--
Bug #22187 (possible crash in number_format() function)
--FILE--
<?php
	var_dump(number_format(0.0001, 1));
	var_dump(number_format(0.0001, 0));
?>
--EXPECT--
unicode(3) "0.0"
unicode(1) "0"
