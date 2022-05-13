--TEST--
IntlBreakIterator factories: basic tests
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "ja");

$m = array('createWordInstance', 'createLineInstance', 'createCharacterInstance',
    'createSentenceInstance', 'createTitleInstance');

$t = 'Frase 1... Frase 2';

foreach ($m as $method) {
    echo "===== $method =====\n";
    $o1 = IntlBreakIterator::$method('ja');
    $o2 = IntlBreakIterator::$method(NULL);
    $o3 = IntlBreakIterator::$method();
    var_dump($o1 == $o2 && $o2 == $o3);
    echo "\n";
}
?>
--EXPECT--
===== createWordInstance =====
bool(true)

===== createLineInstance =====
bool(true)

===== createCharacterInstance =====
bool(true)

===== createSentenceInstance =====
bool(true)

===== createTitleInstance =====
bool(true)
