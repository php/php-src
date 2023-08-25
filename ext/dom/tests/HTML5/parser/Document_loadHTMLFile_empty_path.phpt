--TEST--
DOM\HTML5Document::loadHTMLFile() - empty path
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();

$dom->loadHTMLFile("");
echo $dom->saveHTML(), "\n";

?>
--EXPECTF--
Fatal error: Uncaught ValueError: Path cannot be empty in %s:%d
Stack trace:
#0 %s(%d): DOM\HTML5Document->loadHTMLFile('')
#1 {main}
  thrown in %s on line %d
