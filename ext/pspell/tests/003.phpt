--TEST--
pspell_config_ignore
--SKIPIF--
<?php
if (!extension_loaded('pspell')) die('skip');
if (!@pspell_new('en')) die('skip English dictionary is not available');
?>
--FILE--
<?php

$cfg = pspell_config_create('en', 'british', '', 'iso8859-1');
$cfg2 = pspell_config_create('en', 'british', '', 'b0rked');

$p = pspell_new_config($cfg);
var_dump(pspell_check($p, 'yy'));

$p2 = pspell_new_config($cfg2);
try {
    pspell_check($p2, 'yy');
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

echo "---\n";
var_dump(pspell_config_ignore($cfg, 2));
$p = pspell_new_config($cfg);
var_dump(pspell_check($p, 'yy'));

// segfault it?
var_dump(pspell_config_ignore($cfg, PHP_INT_MAX));

?>
--EXPECTF--
bool(false)

Warning: pspell_new_config(): PSPELL couldn't open the dictionary. reason: The encoding "b0rked" is not known. This could also mean that the file "%sb0rked.%s" could not be opened for reading or does not exist. in %s003.php on line 9
pspell_check(): 0 is not a PSPELL result index
---
bool(true)
bool(true)
bool(true)
