--TEST--
replace empty matches
--POST--
--GET--
--FILE--
<?php echo ereg_replace("^","z","abc123")?>
--EXPECT--
zabc123
