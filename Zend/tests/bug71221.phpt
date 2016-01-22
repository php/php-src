--TEST--
Bug #71221 (Null pointer deref (segfault) in get_defined_vars via ob_start)
--FILE--
<?php 
ob_start("get_defined_vars");
ob_end_clean();
?>
okey
--EXPECT--
okey
