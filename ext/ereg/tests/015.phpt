--TEST--
replace empty matches
--SKIPIF--
<?php if (unicode_semantics()) die('skip unicode.semantics=on'); ?>        
--FILE--
<?php echo ereg_replace("^","z","abc123")?>
--EXPECT--
zabc123
