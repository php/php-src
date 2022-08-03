--TEST--
GH-7734 (IntlPartsIterator key is wrong for KEY_LEFT/KEY_RIGHT)
--EXTENSIONS--
intl
--FILE--
<?php

$iter = \IntlBreakIterator::createCodePointInstance();
$iter->setText('ABC');

foreach ($iter->getPartsIterator(\IntlPartsIterator::KEY_SEQUENTIAL) as $key => $value) {
    var_dump($key, $value);
}

foreach ($iter->getPartsIterator(\IntlPartsIterator::KEY_LEFT) as $key => $value) {
    var_dump($key, $value);
}

foreach ($iter->getPartsIterator(\IntlPartsIterator::KEY_RIGHT) as $key => $value) {
    var_dump($key, $value);
}

?>
--EXPECT--
int(0)
string(1) "A"
int(1)
string(1) "B"
int(2)
string(1) "C"
int(0)
string(1) "A"
int(1)
string(1) "B"
int(2)
string(1) "C"
int(1)
string(1) "A"
int(2)
string(1) "B"
int(3)
string(1) "C"
