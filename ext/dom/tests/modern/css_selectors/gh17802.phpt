--TEST--
GH-17802 (\Dom\HTMLDocument querySelector attribute name is case sensitive in HTML)
--EXTENSIONS--
dom
--FILE--
<?php

$text = <<<TEXT
<html>
<head>
<meta charset="Windows-1252">
</head>
<body>
</body>
</html>
TEXT;

$dom = \Dom\HTMLDocument::createFromString($text, options: LIBXML_NOERROR);
$meta2 = $dom->head->appendChild($dom->createElementNS('urn:x', 'meta'));
$meta2->setAttribute('charset', 'x');
echo $dom->saveHtml(), "\n";

echo "--- charseT ---\n";

foreach ($dom->querySelectorAll('meta[charseT]') as $entry) {
    var_dump($dom->saveHtml($entry));
}

echo "--- charset ---\n";

foreach ($dom->querySelectorAll('meta[charset]') as $entry) {
    var_dump($dom->saveHtml($entry));
}

echo "--- charseT and lowercase value ---\n";

foreach ($dom->querySelectorAll('meta[charseT="windows-1252"]') as $entry) {
    var_dump($dom->saveHtml($entry));
}

echo "--- charset and lowercase value ---\n";

foreach ($dom->querySelectorAll('meta[charset="windows-1252"]') as $entry) {
    var_dump($dom->saveHtml($entry));
}

?>
--EXPECT--
<html><head>
<meta charset="Windows-1252">
<meta charset="x"></meta></head>
<body>

</body></html>
--- charseT ---
string(29) "<meta charset="Windows-1252">"
--- charset ---
string(29) "<meta charset="Windows-1252">"
string(25) "<meta charset="x"></meta>"
--- charseT and lowercase value ---
string(29) "<meta charset="Windows-1252">"
--- charset and lowercase value ---
string(29) "<meta charset="Windows-1252">"
