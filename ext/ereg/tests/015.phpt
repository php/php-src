--TEST--
replace empty matches
--FILE--
<?php echo ereg_replace("^","z","abc123")?>
--EXPECT--
zabc123
