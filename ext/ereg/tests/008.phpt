--TEST--
Test back-references in regular expressions
--FILE--
<?php
	echo ereg_replace("([a-z]*)([-=+|]*)([0-9]+)","\\3 \\1 \\2\n","abc+-|=123");
?>
--EXPECT--
123 abc +-|=
