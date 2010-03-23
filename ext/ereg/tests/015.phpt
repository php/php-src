--TEST--
replace empty matches
--FILE--
<?php echo ereg_replace("^","z","abc123")?>
--EXPECTF--
Deprecated: Function ereg_replace() is deprecated in %s on line %d
zabc123
