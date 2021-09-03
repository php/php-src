--TEST--
pspell configs
--EXTENSIONS--
pspell
--SKIPIF--
<?php
if (!@pspell_new('en')) die('skip English dictionary is not available');
?>
--FILE--
<?php

$cfg = pspell_config_create('en', 'british', '', 'iso8859-1');
var_dump(pspell_config_mode($cfg, PSPELL_BAD_SPELLERS));

var_dump(pspell_config_runtogether($cfg, false));
$p = pspell_new_config($cfg);
var_dump(pspell_check($p, 'theoasis'));

echo "---\n";

// now it should pass
var_dump(pspell_config_runtogether($cfg, true));
$p = pspell_new_config($cfg);
var_dump(pspell_check($p, 'theoasis'));

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
---
bool(true)
bool(true)
