--TEST--
mb_language()
--EXTENSIONS--
mbstring
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
try {
    var_dump(mb_language('Pig Latin'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(mb_language());
?>
--EXPECT--
Checking default language:
string(7) "neutral"
Checking default language after ini_set:
string(3) "uni"
Changing language to English should be successful:
bool(true)
Confirm language was changed:
string(7) "English"
Try changing to a non-existent language:
mb_language(): Argument #1 ($language) must be a valid language, "Pig Latin" given
string(7) "neutral"
