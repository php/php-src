--TEST--
test backslash handling in regular expressions
--SKIPIF--
<?php if (unicode_semantics()) die('skip unicode.semantics=on'); ?>        
--FILE--
<?php echo ereg_replace('\?',"abc","?123?")?>
--EXPECT--
abc123abc
