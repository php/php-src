--TEST--
Interop: simplexml_import_dom
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
<?php if (!extension_loaded("dom")) print "skip. dom extension not loaded"; ?>
--FILE--
<?php 
$dom = new domDocument;
$dom->load(dirname(__FILE__)."/book.xml");
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}
$s = simplexml_import_dom($dom);
$books = $s->book;
foreach ($books as $book) {
	echo "{$book->title} was written by {$book->author}\n";
}
?>
--EXPECT--
The Grapes of Wrath was written by John Steinbeck
The Pearl was written by John Steinbeck
