--TEST--
SPL: Problem with casting to string
--FILE--
<?php
$d = new DirectoryIterator('.');
var_dump($d);
preg_match('/x/', $d);
var_dump(is_string($d));
?>
===DONE===
--EXPECTF--
object(DirectoryIterator)#%d (0) {
}
bool(true)
===DONE===
