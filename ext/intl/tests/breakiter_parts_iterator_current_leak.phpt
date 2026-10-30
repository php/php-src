--TEST--
IntlPartsIterator must not leak, and a temporary one must not dangle
--EXTENSIONS--
intl
--FILE--
<?php
function parts(): IntlPartsIterator {
    $bi = IntlBreakIterator::createWordInstance('en');
    $bi->setText('hello world');
    return $bi->getPartsIterator();
}

foreach (parts() as $part) {
    echo "[$part]\n";
}

$bi = IntlBreakIterator::createWordInstance('en');
$bi->setText('hello world foo bar baz');
$m0 = memory_get_usage();
for ($i = 0; $i < 20000; $i++) {
    foreach ($bi->getPartsIterator() as $v) {
        break;
    }
}
var_dump(memory_get_usage() - $m0 < 1024 * 1024);
?>
--EXPECT--
[hello]
[ ]
[world]
bool(true)
