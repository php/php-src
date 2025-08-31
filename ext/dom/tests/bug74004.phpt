--TEST--
Bug #74004 (DOMDocument->loadHTML and ->loadHTMLFile do not heed LIBXML_NOWARNING and LIBXML_NOERROR options)
--EXTENSIONS--
dom
--FILE--
<?php

$doc=new DOMDocument();
$doc->loadHTML("<tag-throw></tag-throw>",LIBXML_NOERROR);

?>
===DONE===
--EXPECT--
===DONE===
