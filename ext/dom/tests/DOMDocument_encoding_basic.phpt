--TEST--
DOMDocument::$encoding - read/write tests (dom_document_encoding_read/dom_document_encoding_write)
--CREDITS--
Hans Zaunere
# TestFest 2009 NYPHP
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('dom_test.inc');

$dom = new DOMDocument;
$dom->loadXML($xmlstr);

if( !$dom )
{
    echo "Error while parsing the document\n";
    exit;
}

echo "Empty Encoding Read: {$dom->encoding}\n";

$ret = $dom->encoding = 'NYPHP DOMinatrix';
echo "Adding invalid encoding: $ret\n";

$ret = $dom->encoding = 'ISO-8859-1';
echo "Adding ISO-8859-1 encoding: $ret\n";
echo "ISO-8859-1 Encoding Read: {$dom->encoding}\n";

$ret = $dom->encoding = 'UTF-8';
echo "Adding UTF-8 encoding: $ret\n";
echo "UTF-8 Encoding Read: {$dom->encoding}\n";

$ret = $dom->encoding = 'UTF-16';
echo "Adding UTF-16 encoding: $ret\n";
echo "UTF-16 Encoding Read: {$dom->encoding}\n";


?>
--EXPECTF--
Empty Encoding Read: 

Warning: main(): Invalid Document Encoding in %s on line %d
Adding invalid encoding: NYPHP DOMinatrix
Adding ISO-8859-1 encoding: ISO-8859-1
ISO-8859-1 Encoding Read: ISO-8859-1
Adding UTF-8 encoding: UTF-8
UTF-8 Encoding Read: UTF-8
Adding UTF-16 encoding: UTF-16
UTF-16 Encoding Read: UTF-16

