--TEST--
Bug #81377: unset() of $GLOBALS sub-key yields warning
--FILE--
<?php

unset($GLOBALS['foo']['bar']);

?>
===DONE==
--EXPECT--
===DONE==
