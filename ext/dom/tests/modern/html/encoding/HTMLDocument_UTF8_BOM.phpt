--TEST--
DOM\HTMLDocument UTF-8 BOM encoding test
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromFile(__DIR__ . "/utf8_bom.html");
var_dump($dom->charset);
$dom->documentElement->firstChild->nextElementSibling->textContent = "é";
$output = $dom->saveHTML();
echo $output, "\n";
$dom->saveHTMLFile(__DIR__ . "/utf8_bom_output.tmp");
var_dump(file_get_contents(__DIR__ . "/utf8_bom_output.tmp") === $output);

echo "--- After changing encoding to UTF-8 ---\n";
$dom->charset = "UTF-8";
echo $dom->saveHTML(), "\n";

?>
--CLEAN--
<?php
@unlink(__DIR__ . "/utf8_bom_output.tmp");
?>
--EXPECT--
string(5) "UTF-8"
<!DOCTYPE html><html><head>
<!-- intentional lies and deceit -->
<meta charset="utf-16">
</head>
<body>é</body></html>
bool(true)
--- After changing encoding to UTF-8 ---
<!DOCTYPE html><html><head>
<!-- intentional lies and deceit -->
<meta charset="utf-16">
</head>
<body>é</body></html>
