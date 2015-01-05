--TEST--
Testing string scanner confirmance
--FILE--
<?php echo "\"\t\\'" . '\n\\\'a\\\b\\' ?>
--EXPECT--
"	\'\n\'a\\b\
