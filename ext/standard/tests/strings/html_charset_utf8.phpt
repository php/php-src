--TEST--
Charset names that are UTF-8 or close to it
--FILE--
<?php
foreach (['UTF-8', 'utf-8', 'uTf-8', 'UTF8', 'UTF-', 'UTF-8 ', 'UTF-88', 'UTF_8'] as $charset) {
    $escaped = htmlspecialchars("\xC3\xA9<", ENT_QUOTES, $charset);
    echo var_export($charset, true), ": $escaped\n";
}

ini_set('default_charset', 'uTF-8');
echo htmlspecialchars("\xC3\xA9<", ENT_QUOTES), "\n";
?>
--EXPECTF--
'UTF-8': é&lt;
'utf-8': é&lt;
'uTf-8': é&lt;

Warning: htmlspecialchars(): Charset "UTF8" is not supported, assuming UTF-8 in %s on line %d
'UTF8': é&lt;

Warning: htmlspecialchars(): Charset "UTF-" is not supported, assuming UTF-8 in %s on line %d
'UTF-': é&lt;

Warning: htmlspecialchars(): Charset "UTF-8 " is not supported, assuming UTF-8 in %s on line %d
'UTF-8 ': é&lt;

Warning: htmlspecialchars(): Charset "UTF-88" is not supported, assuming UTF-8 in %s on line %d
'UTF-88': é&lt;

Warning: htmlspecialchars(): Charset "UTF_8" is not supported, assuming UTF-8 in %s on line %d
'UTF_8': é&lt;
é&lt;
