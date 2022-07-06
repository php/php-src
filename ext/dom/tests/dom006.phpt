--TEST--
Test 6: Extends Test
--SKIPIF--
<?php  require_once('skipif.inc'); ?>
--FILE--
<?php

Class books extends domDocument {
    function addBook($title, $author) {
        $titleElement = $this->createElement("title");
        $titleElement->appendChild($this->createTextNode($title));
        $authorElement = $this->createElement("author");
        $authorElement->appendChild($this->createTextNode($author));

        $bookElement = $this->createElement("book");

        $bookElement->appendChild($titleElement);
        $bookElement->appendChild($authorElement);
        $this->documentElement->appendChild($bookElement);
    }

}

$dom = new books;

$dom->load(__DIR__."/book.xml");
$dom->addBook("PHP de Luxe", "Richard Samar, Christian Stocker");
print $dom->saveXML();
?>
--EXPECT--
<?xml version="1.0"?>
<books>
 <book>
  <title>The Grapes of Wrath</title>
  <author>John Steinbeck</author>
 </book>
 <book>
  <title>The Pearl</title>
  <author>John Steinbeck</author>
 </book>
<book><title>PHP de Luxe</title><author>Richard Samar, Christian Stocker</author></book></books>
