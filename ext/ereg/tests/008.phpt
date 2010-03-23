--TEST--
Test back-references in regular expressions
--FILE--
<?php
	echo ereg_replace("([a-z]*)([-=+|]*)([0-9]+)","\\3 \\1 \\2\n","abc+-|=123");
?>
--EXPECTF--
Deprecated: Function ereg_replace() is deprecated in %s on line %d
123 abc +-|=
