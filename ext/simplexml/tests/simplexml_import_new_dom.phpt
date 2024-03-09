--TEST--
SimpleXML [interop]: simplexml_import_dom (new DOM)
--EXTENSIONS--
simplexml
dom
--FILE--
<?php
$dom = DOM\XMLDocument::createFromFile(__DIR__."/book.xml");
$s = simplexml_import_dom($dom);
$books = $s->book;
foreach ($books as $book) {
    echo "{$book->title} was written by {$book->author}\n";
}

$s->book[0]->title = "test";
echo $dom->saveXML();

?>
--EXPECT--
The Grapes of Wrath was written by John Steinbeck
The Pearl was written by John Steinbeck
<?xml version="1.0" encoding="UTF-8"?>
<books>
 <book>
  <title>test</title>
  <author>John Steinbeck</author>
 </book>
 <book>
  <title>The Pearl</title>
  <author>John Steinbeck</author>
 </book>
</books>
