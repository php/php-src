--TEST--
033: Import statement with non-compound name
--FILE--
<?php
import A;
--EXPECTF--
Warning: The import statement with non-compound name 'A' has no effect in %sns_033.php on line 2

