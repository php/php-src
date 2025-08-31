--TEST--
Document::$implementation createHTMLDocument
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createEmpty();

echo "---\n";

echo $dom->implementation->createHTMLDocument()->saveXml(), "\n";

echo "---\n";

echo $dom->implementation->createHTMLDocument("")->saveXml(), "\n";

echo "---\n";

echo $dom->implementation->createHTMLDocument("my <title>")->saveXml(), "\n";

?>
--EXPECT--
---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body></body></html>
---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml"><head><title></title></head><body></body></html>
---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml"><head><title>my &lt;title&gt;</title></head><body></body></html>
