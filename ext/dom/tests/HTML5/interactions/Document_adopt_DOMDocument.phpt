--TEST--
DOM\HTML5Document adopts a DOMDocument
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadHTML(<<<HTML
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
</head>
<body>
</body>
</html>
HTML);

$dom2 = new DOM\HTML5Document();
$dom2->appendChild($dom2->adoptNode($dom->documentElement));
echo $dom2->saveHTML();

?>
--EXPECT--
<html>
<head>
</head>
<body>
</body>
</html>
