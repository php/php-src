<?php
$dom = new domDocument;
$dom->load("book.xml");
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}
print "As SimpleXML\n";

$s = simplexml_import_dom($dom);
$books = $s->book;
foreach ($books as $book) {
	echo "{$book->title} was written by {$book->author}\n";
}

print "As DOM \n";

$dom = dom_import_simplexml($s);
$books = $dom->getElementsByTagName("book");
foreach ($books as $book) {
       $title = $book->getElementsByTagName("title");
       $author = $book->getElementsByTagName("author");
       echo $title[0]->firstChild->data . " was written by ". $author[0]->firstChild->data . "\n";
}


?>
