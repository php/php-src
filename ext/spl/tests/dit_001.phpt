--TEST--
SPL: Problem with casting to string
--FILE--
<?php
$d = new DirectoryIterator('.');
preg_match('/x/', $d);
var_dump($d);
?>
--EXPECTF--
object(DirectoryIterator)#%d (0) {
}
