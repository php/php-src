--TEST--
IntlCodePointBreakIterator: clone and equality
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$text = 'ตัวอย่างข้อความ';
$text2 = 'foo';

$it = IntlBreakIterator::createCodePointInstance();
$it->setText($text);

$it_clone = clone $it;
var_dump($it == $it_clone);

$it->setText($text2 );
var_dump($it == $it_clone);

$it_clone->setText($text2);
var_dump($it == $it_clone);

?>
--EXPECT--
bool(true)
bool(false)
bool(true)
