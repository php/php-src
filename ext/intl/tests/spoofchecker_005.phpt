--TEST--
spoofchecker with settings changed
--SKIPIF--
<?php if(!extension_loaded('intl') || !class_exists("Spoofchecker")) print 'skip'; ?>
<?php if (version_compare(INTL_ICU_VERSION, '55.1') < 0) die('skip for ICU >= 55.1'); ?>
--FILE--
<?php

$korean = "\xED\x95\x9C" . "\xEA\xB5\xAD" . "\xEB\xA7\x90";

$x = new Spoofchecker();
echo "Check with default settings\n";
var_dump($x->areConfusable("HELLO", "H\xD0\x95LLO"));
var_dump($x->areConfusable("hello", "h\xD0\xB5llo"));

echo "Change confusable settings\n";
$x->setChecks(Spoofchecker::MIXED_SCRIPT_CONFUSABLE |
  Spoofchecker::WHOLE_SCRIPT_CONFUSABLE |
  Spoofchecker::SINGLE_SCRIPT_CONFUSABLE);
var_dump($x->areConfusable("HELLO", "H\xD0\x95LLO"));
var_dump($x->areConfusable("hello", "h\xD0\xB5llo"));
?>
--EXPECTF--
Check with default settings
bool(true)
bool(true)
Change confusable settings
bool(true)
bool(true)
