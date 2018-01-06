--TEST--
SCCP 016: Used constant instructions should be replaced with QM_ASSIGN
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
return isset($undef) || php_sapi_name() == php_sapi_name();
?>
--EXPECTF--
$_main: ; (lines=4, args=0, vars=1, tmps=1)
    ; (after optimizer)
    ; %ssccp_016.php:1-4
L0 (2):     T1 = ISSET_ISEMPTY_CV (isset) CV0($undef)
L1 (2):     T1 = JMPNZ_EX T1 L3
L2 (2):     T1 = QM_ASSIGN bool(true)
L3 (2):     RETURN T1
