--TEST--
DOM\HTMLDocument GB18030 encoding test
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromFile(__DIR__ . "/gb18030.html");
var_dump($dom->encoding);
$dom->documentElement->firstChild->nextElementSibling->textContent = "茅";
$output = $dom->saveHTML();
echo $output, "\n";
$dom->saveHTMLFile(__DIR__ . "/gb18030_output.tmp");
var_dump(file_get_contents(__DIR__ . "/gb18030_output.tmp") === $output);

echo "--- After changing encoding to UTF-8 ---\n";
$dom->encoding = "UTF-8";
echo $dom->saveHTML(), "\n";

?>
--CLEAN--
<?php
@unlink(__DIR__ . "/gb18030_output.tmp");
?>
--EXPECT--
string(7) "gb18030"
<!DOCTYPE html><html><head>
<meta charset="gb18030">
</head>
<body>é</body></html>
bool(true)
--- After changing encoding to UTF-8 ---
<!DOCTYPE html><html><head>
<meta charset="gb18030">
</head>
<body>茅</body></html>
