--TEST--
pcntl_setpriority() - Basic behaviour
--CREDITS--
Er Galvão Abbott galvao@galvao.eti.br
# TestFest 2017 PHPRS PHP UG 2017-10-31
--EXTENSIONS--
pcntl
--SKIPIF--
<?php

if (!function_exists('pcntl_setpriority')) {
    die('skip pcntl_setpriority doesn\'t exist');
}
if (@pcntl_setpriority(-5) === false && pcntl_get_last_error() == PCNTL_EACCES) {
    die('skip this function needs to run with CAP_SYS_NICE privileges');
}
?>
--FILE--
<?php
var_dump(pcntl_setpriority(-5));
?>
--EXPECT--
bool(true)
