--TEST--
Bug GH-18756: Zend MM may delete the main chunk
--EXTENSIONS--
zend_test
--FILE--
<?php

zend_test_gh18756();

?>
==DONE==
--EXPECT--
==DONE==
