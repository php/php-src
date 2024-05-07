--TEST--
Dom\HTMLDocument::createFromFile() - local existing file
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromFile(__DIR__ . "/../../../test.html");
echo $dom->saveHtml(), "\n";

?>
--EXPECTF--
Warning: Dom\HTMLDocument::createFromFile(): tree error unexpected-token-in-initial-mode in %s on line %d
<html><head>
<title>Hello world</title>
</head>
<body>
This is a not well-formed<br>
html files with undeclared entities&nbsp;


</body></html>
