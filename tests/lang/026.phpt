--TEST--
Testing string scanner confirmance
--POST--
--GET--
--FILE--
<?php echo "\"\t\\'" . '\n\\\'a\\\b\\' ?>
--EXPECT--
"	\'\n\'a\\b\
