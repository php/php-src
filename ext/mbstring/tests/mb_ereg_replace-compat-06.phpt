--TEST--
mb_ereg_replace() compatibility test 6 (counterpart: ext/standard/tests/reg/008.phpt)
--POST--
--GET--
--FILE--
<?php
	echo mb_ereg_replace("([a-z]*)([-=+|]*)([0-9]+)","\\3 \\1 \\2\n","abc+-|=123");
?>
--EXPECT--
123 abc +-|=
