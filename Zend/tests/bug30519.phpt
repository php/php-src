--TEST--
Bug #30519 (Interface not existing says Class not found)
--FILE--
<?php
class test implements a {
}
?>
--EXPECTF--
Fatal error: Interface 'a' not found in %sbug30519.php on line 2
