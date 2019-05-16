--TEST--
mb_language()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip');
--INI--
mbstring.language=
--FILE--
<?php
echo "Checking default language:\n";
var_dump(mb_language());

echo "Checking default language after ini_set:\n";
ini_set('mbstring.language', 'uni');
var_dump(mb_language());

echo "Changing language to English should be successful:\n";
var_dump(mb_language('English'));

echo "Confirm language was changed:\n";
var_dump(mb_language());

echo "Try changing to a non-existent language:\n";
var_dump(mb_language('Pig Latin'));
var_dump(mb_language());
?>
--EXPECTF--
Checking default language:
string(7) "neutral"
Checking default language after ini_set:
string(3) "uni"
Changing language to English should be successful:
bool(true)
Confirm language was changed:
string(7) "English"
Try changing to a non-existent language:

Warning: mb_language(): Unknown language "Pig Latin" in %s on line %d
bool(false)
string(7) "neutral"
