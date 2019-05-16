--TEST--
Bug #71220 (Null pointer deref (segfault) in compact via ob_start)
--FILE--
<?php
ob_start("compact");
ob_end_clean();
?>
okey
--EXPECT--
okey
