--TEST--
DOM\HTML5Document::loadHTMLFile() - local existing file
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();

$dom->loadHTMLFile(__DIR__ . "/../../test.html");
echo $dom->saveHTML(), "\n";

?>
--EXPECTF--
Warning: DOM\HTML5Document::loadHTMLFile(): tree error unexpected-token-in-initial-mode in %s on line %d
<html><head>
<title>Hello world</title>
</head>
<body>
This is a not well-formed<br>
html files with undeclared entities&nbsp;


</body></html>
