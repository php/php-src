--TEST--
pspell session
--SKIPIF--
<?php
if (!extension_loaded('pspell')) die('skip');
if (!@pspell_new('en')) die('skip English dictionary is not available');
?>
--FILE--
<?php

$p = pspell_new('en');

var_dump(pspell_check('a'));
var_dump(pspell_check($p, 'somebogusword'));

var_dump(pspell_add_to_session($p, ''));
var_dump(pspell_add_to_session($p, 'somebogusword'));
var_dump(pspell_check($p, 'somebogusword'));

var_dump(pspell_clear_session(new stdclass));

$res = @pspell_clear_session($p);
if ($res) {
	var_dump($res);
	var_dump(pspell_check($p, 'somebogusword'));
} else {
	echo "bool(true)\n";
	echo "bool(false)\n";
}
?>
--EXPECTF--
Warning: pspell_check() expects exactly 2 parameters, 1 given in %s on line %d
NULL
bool(false)
bool(false)
bool(true)
bool(true)

Warning: pspell_clear_session() expects parameter 1 to be integer, object given in %s on line %d
NULL
bool(true)
bool(false)
