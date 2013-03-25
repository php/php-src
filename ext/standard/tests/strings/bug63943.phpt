--TEST--
Bug #63943 (Bad warning text from strpos() on empty needle)
--FILE--
<?php
strpos("lllllll", '');
?>
--EXPECTF--
Warning: strpos(): Empty needle in %sbug63943.php on line %d
