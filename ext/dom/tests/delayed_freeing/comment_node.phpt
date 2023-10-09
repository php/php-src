--TEST--
Delayed freeing comment node
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$comment = $doc->appendChild($doc->createElement('container'))
    ->appendChild($doc->createComment('my comment'));
echo $doc->saveXML(), "\n";
$comment->parentNode->remove();
echo $doc->saveXML(), "\n";
echo $doc->saveXML($comment), "\n";
var_dump($comment->parentNode);
?>
--EXPECT--
<?xml version="1.0"?>
<container><!--my comment--></container>

<?xml version="1.0"?>

<!--my comment-->
NULL
