--TEST--
033: Import statement with non-compound name
--FILE--
<?php
use A;
use \B;
?>
--EXPECTF--
Warning: The use statement with non-compound name 'A' has no effect in %sns_033.php on line 2

Warning: The use statement with non-compound name 'B' has no effect in %sns_033.php on line 3
