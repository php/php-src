--TEST--
IntlBreakIterator::getPartsIterator(): bad args
--EXTENSIONS--
intl
--FILE--
<?php

$it = IntlBreakIterator::createWordInstance(NULL);

try {
    var_dump($it->getPartsIterator(-1));
} catch(\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
IntlBreakIterator::getPartsIterator(): Argument #1 ($type) must be one of IntlPartsIterator::KEY_SEQUENTIAL, IntlPartsIterator::KEY_LEFT, or IntlPartsIterator::KEY_RIGHT
