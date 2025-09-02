--TEST--
pcntl_getpriority() - Basic behaviour
--CREDITS--
Er Galvão Abbott galvao@galvao.eti.br
# TestFest 2017 PHPRS PHP UG 2017-10-29
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('pcntl_getpriority')) {
    die('skip - pcntl_getpriority doesn\'t exist');
}
?>
--FILE--
<?php
var_dump(pcntl_getpriority());
?>
--EXPECTF--
int(%i)
