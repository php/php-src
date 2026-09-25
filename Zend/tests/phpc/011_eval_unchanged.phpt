--TEST--
.phpc: eval() is unaffected — string compile path is independent of file extension
--FILE--
<?php
/* eval() always uses ZEND_COMPILE_POSITION_AFTER_OPEN_TAG, which already
 * starts in ST_IN_SCRIPTING. .phpc semantics live in the file-compile
 * path and must not leak into the string-compile path. */
eval('echo "eval-ok\n";');
?>
--EXPECT--
eval-ok
