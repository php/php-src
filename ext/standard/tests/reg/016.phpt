--TEST--
test backslash handling in regular expressions
--POST--
--GET--
--FILE--
<?php echo ereg_replace('\?',"abc","?123?")?>
--EXPECT--
abc123abc
