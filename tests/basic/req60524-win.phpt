--TEST--
Req #60524 (Specify temporary directory)
--INI--
sys_temp_dir=C:\Windows
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != "WIN" )
  die('skip Run only on Windows');
?>
--FILE--
<?php echo sys_get_temp_dir(); ?>
--EXPECT--
C:\\Windows
