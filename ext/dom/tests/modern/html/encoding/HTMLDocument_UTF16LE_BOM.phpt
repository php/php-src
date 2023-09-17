--TEST--
DOM\HTMLDocument UTF-16LE BOM encoding test
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromFile(__DIR__ . "/utf16le_bom.html");
var_dump($dom->encoding);
$dom->documentElement->firstChild->nextElementSibling->textContent = "é";
$output = $dom->saveHTML();
echo $output, "\n";
$dom->saveHTMLFile(__DIR__ . "/utf16le_bom_output.tmp");
var_dump(file_get_contents(__DIR__ . "/utf16le_bom_output.tmp") === $output);

echo "--- After changing encoding to UTF-8 ---\n";
$dom->encoding = "UTF-8";
echo $dom->saveHTML(), "\n";

?>
--CLEAN--
<?php
@unlink(__DIR__ . "/utf16le_bom_output.tmp");
?>
--EXPECTF--
string(8) "UTF-16LE"
<%0!%0D%0O%0C%0T%0Y%0P%0E%0 %0h%0t%0m%0l%0>%0<%0h%0t%0m%0l%0>%0<%0h%0e%0a%0d%0>%0
%0<%0!%0-%0-%0 %0i%0n%0t%0e%0n%0t%0i%0o%0n%0a%0l%0 %0l%0i%0e%0s%0 %0a%0n%0d%0 %0d%0e%0c%0e%0i%0t%0 %0-%0-%0>%0
%0<%0m%0e%0t%0a%0 %0c%0h%0a%0r%0s%0e%0t%0=%0"%0u%0t%0f%0-%08%0"%0>%0
%0<%0/%0h%0e%0a%0d%0>%0
%0<%0b%0o%0d%0y%0>%0�%0<%0/%0b%0o%0d%0y%0>%0<%0/%0h%0t%0m%0l%0>%0
bool(true)
--- After changing encoding to UTF-8 ---
<!DOCTYPE html><html><head>
<!-- intentional lies and deceit -->
<meta charset="utf-8">
</head>
<body>é</body></html>
