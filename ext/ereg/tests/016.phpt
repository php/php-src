--TEST--
test backslash handling in regular expressions
--FILE--
<?php echo ereg_replace('\?',"abc","?123?")?>
--EXPECTF--
Deprecated: Function ereg_replace() is deprecated in %s on line %d
abc123abc
