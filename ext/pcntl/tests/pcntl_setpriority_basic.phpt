--TEST--
pcntl_setpriority() - Basic behaviour
--CREDITS--
Er Galvão Abbott galvao@galvao.eti.br
# TestFest 2017 PHPRS PHP UG 2017-10-31
--SKIPIF--
<?php
if (!isset($_SERVER['SUDO_USER'])) {
    die('skip - this functions needs to run with superuser');
}

if (!extension_loaded('pcntl')) {
    die('skip - ext/pcntl not loaded');
} else if (!function_exists('pcntl_setpriority')) {
    die('skip - pcntl_setpriority doesn\'t exist');
}
?>
--FILE--
<?php
var_dump(pcntl_setpriority(-5));
?>
--EXPECT--
bool(true)
