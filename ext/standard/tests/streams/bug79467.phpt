--TEST--
Bug #79467 (data:// wrappers are writable)
--FILE--
<?php
var_dump(file_put_contents('data://text/plain,cccc', 'data'));
?>
--EXPECTF--
Notice: file_put_contents(): Stream is not writable in %s on line %d
bool(false)
