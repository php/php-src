--TEST--
DOM\HTML5Document::loadHTMLFile() - local file that does not exist
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();

$dom->loadHTMLFile(__DIR__ . "/foobar");
echo $dom->saveHTML(), "\n";

?>
--EXPECTF--
Warning: DOM\HTML5Document::loadHTMLFile(%s): Failed to open stream: No such file or directory in %s on line %d
