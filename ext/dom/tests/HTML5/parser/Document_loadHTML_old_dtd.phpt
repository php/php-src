--TEST--
HTML5Document::loadHTML(): Old DTD
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$dom->loadHTML(<<<HTML
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
Warning: DOM\HTML5Document::loadHTML(): tree error bad-doctype-token-in-initial-mode in Entity, line: 1, column: 3-9 in %s on line %d
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
