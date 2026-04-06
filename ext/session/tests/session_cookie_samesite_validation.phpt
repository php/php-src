--TEST--
session.cookie_samesite only accepts Strict, Lax, None, or empty string
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
ob_start();
ini_set('session.cookie_samesite', 'Invalid');
ini_set('session.cookie_samesite', "Strict\r\nX-Injected: evil");
$after_invalid = ini_get('session.cookie_samesite');

$r_strict = ini_set('session.cookie_samesite', 'Strict') !== false;
$r_lax    = ini_set('session.cookie_samesite', 'Lax')    !== false;
$r_none   = ini_set('session.cookie_samesite', 'None')   !== false;
$r_empty  = ini_set('session.cookie_samesite', '')       !== false;
echo ob_get_clean();

var_dump($after_invalid);
var_dump($r_strict, $r_lax, $r_none, $r_empty);
?>
--EXPECTF--
Warning: ini_set(): session.cookie_samesite must be "Strict", "Lax", "None", or "" in %s on line %d

Warning: ini_set(): session.cookie_samesite must be "Strict", "Lax", "None", or "" in %s on line %d
string(0) ""
bool(true)
bool(true)
bool(true)
bool(true)
