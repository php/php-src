--TEST--
Test dumping scalar extensions in opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache')) die('skip Zend OPcache extension not available'); ?>
--FILE--
<?php

use extension array ArrayExtension;
use extension string StringExtension;

class ArrayExtension {}

--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s002.php:1-8
     ; (use extension string StringExtension)
     ; (use extension array ArrayExtension)
0000 RETURN int(1)
