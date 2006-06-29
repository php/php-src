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
Warning: Wrong parameter count for pspell_check() in %s002.php on line 5
NULL
bool(false)
bool(false)
bool(true)
bool(true)

Notice: Object of class stdClass could not be converted to int in %s002.php on line 12

Warning: pspell_clear_session(): 1 is not a PSPELL result index in %s002.php on line 12
bool(false)
bool(true)
bool(false)
