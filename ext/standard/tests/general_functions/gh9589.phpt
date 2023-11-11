--TEST--
dl() segfaults when module is already loaded
--EXTENSIONS--
dl_test
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('skip dl() crashes LSan');
?>
--FILE--
<?php
dl("dl_test");
?>
--EXPECT--
Warning: Module "dl_test" is already loaded in Unknown on line 0
