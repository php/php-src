--TEST--
Req #60524 (Specify temporary directory)
--INI--
sys_temp_dir=/path/to/temp/dir
--FILE--
<?php echo sys_get_temp_dir(); ?>
--EXPECT--
/path/to/temp/dir
