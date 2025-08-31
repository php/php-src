--TEST--
Testing string scanner conformance
--FILE--
<?php echo "\"\t\\'" . '\n\\\'a\\\b\\' ?>
--EXPECT--
"	\'\n\'a\\b\
