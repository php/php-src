--TEST--
Dom\HTMLDocument Windows-1251 encoding test
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromFile(__DIR__ . "/windows1251.html");
var_dump($dom->charset);
$dom->documentElement->firstChild->nextElementSibling->textContent .= "é"; // Note: won't show up in Windows 1251 because it doesn't exist there
$output = $dom->saveHtml();
echo $output, "\n";
$dom->saveHtmlFile(__DIR__ . "/windows1251_output.tmp");
var_dump(file_get_contents(__DIR__ . "/windows1251_output.tmp") === $output);

echo "--- After changing encoding to UTF-8 ---\n";
$dom->charset = "UTF-8";
echo $dom->saveHtml(), "\n";

?>
--CLEAN--
<?php
@unlink(__DIR__ . "/windows1251_output.tmp");
?>
--EXPECT--
string(12) "windows-1251"
<!DOCTYPE html><html><head>
<meta charset="windows-1251">
</head>
<body>
    A � B � C
?</body></html>
bool(true)
--- After changing encoding to UTF-8 ---
<!DOCTYPE html><html><head>
<meta charset="windows-1251">
</head>
<body>
    A ф B б C
é</body></html>
