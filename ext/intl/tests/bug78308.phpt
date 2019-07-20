--TEST--
Bug #78308 (IntlPartsIterator key is wrong for KEY_LEFT/KEY_RIGHT)
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip intl extension not available');
?>
--FILE--
<?php
$iter = \IntlBreakIterator::createCodePointInstance();
$iter->setText('ABC');
echo "KEY_LEFT:\n";
foreach ($iter->getPartsIterator(\IntlPartsIterator::KEY_LEFT) as $key => $value) {
    var_dump($key, $value);
}
echo "KEY_RIGHT:\n";
foreach ($iter->getPartsIterator(\IntlPartsIterator::KEY_RIGHT) as $key => $value) {
    var_dump($key, $value);
}
?>
--EXPECT--
KEY_LEFT:
int(0)
string(1) "A"
int(1)
string(1) "B"
int(2)
string(1) "C"
KEY_RIGHT:
int(1)
string(1) "A"
int(2)
string(1) "B"
int(3)
string(1) "C"
