--TEST--
A script with die() must end "normally"
--SKIPIF--
<?php
if (ini_get('opcache.jit') && ini_get('opcache.jit_buffer_size')) {
    die('skip phpdbg is incompatible with JIT');
}
?>
--PHPDBG--
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Script ended normally]
prompt> 
--FILE--
<?php

(function($argv) {
	die();
})($argv);
