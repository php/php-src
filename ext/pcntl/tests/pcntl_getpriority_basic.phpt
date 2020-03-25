--TEST--
pcntl_getpriority() - Basic behaviour
--CREDITS--
Er Galvão Abbott galvao@galvao.eti.br
# TestFest 2017 PHPRS PHP UG 2017-10-29
--SKIPIF--
<?php
if (!extension_loaded('pcntl')) {
    die('skip - ext/pcntl not loaded');
} else if (!function_exists('pcntl_getpriority')) {
    die('skip - pcntl_getpriority doesn\'t exist');
}
?>
--FILE--
<?php
var_dump(pcntl_getpriority());
?>
--EXPECTF--
int(%d)
