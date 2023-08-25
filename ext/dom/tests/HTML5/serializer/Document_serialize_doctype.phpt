--TEST--
DOM\HTML5Document serialization of document type
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$dom->loadHTML(<<<HTML
<!DOCTYPE HTML1234 PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
</head>
<body>
</body>
</html>
HTML, LIBXML_NOERROR);

echo "--- XML encoding ---\n";
echo $dom->saveXML();
echo "--- HTML encoding ---\n";
// We don't expec to see the public ID and the system ID because the serialization algorithm doesn't serialize those
echo $dom->saveHTML();

?>
--EXPECT--
--- XML encoding ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!DOCTYPE html1234 PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><head>
</head>
<body>

</body></html>
--- HTML encoding ---
<!DOCTYPE html1234><html><head>
</head>
<body>

</body></html>
