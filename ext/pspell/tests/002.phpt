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

var_dump(pspell_check($p, 'somebogusword'));

var_dump(pspell_add_to_session($p, ''));
var_dump(pspell_add_to_session($p, 'somebogusword'));
var_dump(pspell_check($p, 'somebogusword'));

$res = @pspell_clear_session($p);
if ($res) {
    var_dump($res);
    var_dump(pspell_check($p, 'somebogusword'));
} else {
    echo "bool(true)\n";
    echo "bool(false)\n";
}
?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
