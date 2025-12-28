--TEST--
Dom\HTMLDocument fallback encoding test
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromFile(__DIR__ . "/fallback_encoding.html");
var_dump($dom->inputEncoding);
echo $dom->saveHtml();

?>
--EXPECT--
string(5) "UTF-8"
<!DOCTYPE html><html><head>
<meta charset="I don't exist">
</head>
<body>
</body></html>
