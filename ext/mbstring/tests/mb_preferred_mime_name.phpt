--TEST--
mb_preferred_mime_name()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
// TODO: Add more encoding names

$encodings = [
    'sjis-win',
    'SJIS',
    'EUC-JP',
    'UTF-8',
    'ISO-2022-JP',
    'JIS',
    'ISO-8859-1',
    'UCS2',
    'UCS4',
    // Invalid
    'BAD_NAME',
    // No preferred name
    'pass',
];

foreach($encodings as $encoding) {
    try {
        $str = mb_preferred_mime_name($encoding);
        echo $str . \PHP_EOL;
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}

?>
--EXPECT--
Shift_JIS
Shift_JIS
EUC-JP
UTF-8
ISO-2022-JP
ISO-2022-JP
ISO-8859-1
UCS-2
UCS-4
Unknown encoding "BAD_NAME"
No MIME preferred name corresponding to "pass"
