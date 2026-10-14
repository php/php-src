--TEST--
Charset names are case-insensitive, and names longer than the longest one are not supported
--FILE--
<?php
foreach (['WINDOWS-1252', 'wInDoWs-1252', 'MACROMAN', 'Windows-12520', 'MacRoman-but-longer'] as $charset) {
    $decoded = html_entity_decode('&eacute;', ENT_QUOTES, $charset);
    echo var_export($charset, true), ': ', bin2hex($decoded), "\n";
}
?>
--EXPECTF--
'WINDOWS-1252': e9
'wInDoWs-1252': e9
'MACROMAN': 8e

Warning: html_entity_decode(): Charset "Windows-12520" is not supported, assuming UTF-8 in %s on line %d
'Windows-12520': c3a9

Warning: html_entity_decode(): Charset "MacRoman-but-longer" is not supported, assuming UTF-8 in %s on line %d
'MacRoman-but-longer': c3a9
