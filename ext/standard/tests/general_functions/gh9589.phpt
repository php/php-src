--TEST--
dl() segfaults when module is already loaded
--EXTENSIONS--
dl_test
--FILE--
<?php
dl("dl_test");
?>
--EXPECT--
Warning: Module "dl_test" is already loaded in Unknown on line 0
