--TEST--
DOM\HTMLDocument::createFromString(): Old DTD
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString(<<<HTML
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
</head>
<body>
</body>
</html>
HTML);

echo "--- HTML serialization ---\n";
echo $dom->saveHTML(), "\n";
echo "--- XML serialization ---\n";
echo $dom->saveXML();

?>
--EXPECTF--
Warning: DOM\HTMLDocument::createFromString(): tree error bad-doctype-token-in-initial-mode in Entity, line: 1, column: 3-9 in %s on line %d
--- HTML serialization ---
<!DOCTYPE html><html><head>
</head>
<body>

</body></html>
--- XML serialization ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><head>
</head>
<body>

</body></html>
