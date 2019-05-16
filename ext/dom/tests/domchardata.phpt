--TEST--
CharData: DOMCharacterData and related functionality
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML($xmlstr);
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

$node = $dom->documentElement;

$charnode = $dom->createElement('charnode');
$node->appendChild($charnode);

/* DOMComment */
$comment = new DOMComment('Testing character data and extending nodes');
$charnode->appendChild($comment);

echo "Comment Length: ".$comment->length."\n";

$comment->data = 'Updated comment';
echo "New Comment Length: ".$comment->length."\n";
echo "New Comment Data: ".$comment->data."\n";

/* DOMCDataSection */
$cdata = new DOMCDataSection('Chars: <>&"');
$charnode->appendChild($cdata);

echo "Substring: ".$cdata->substringData(7, 4)."\n";
$cdata->replaceData(10, 1, "'");
echo "New Substring: ".$cdata->substringData(7, 4)."\n";

/* DOMCharacterData using DOMComment */
$comment = new DOMComment('instructions');
echo "Comment Value: ".$comment->data."\n";
$comment->data = 'some more instructions';
echo "New Comment Value: ".$comment->data."\n";

$comment->insertData(10, 'pi ');
$comment->replaceData(18, 5, 'i');
$comment->insertData(20, 'g');
$comment->deleteData(13, 2);
$comment->deleteData(10, 3);
$comment->insertData(10, 'comment ');
echo "Updated Comment Value: ".$comment->data."\n";

/* DOMText */
$text = new DOMText('some text characters');

echo "Whole Text: ".$text->wholeText."\n";
$text2 = $text->splitText(9);

echo "Split text: ".$text2->wholeText."\n";
$text3 = $text2->splitText(1);

echo "Is Whitespace?: ".($text2->isElementContentWhitespace()?'YES':'NO');
?>
--EXPECT--
Comment Length: 42
New Comment Length: 15
New Comment Data: Updated comment
Substring: <>&"
New Substring: <>&'
Comment Value: instructions
New Comment Value: some more instructions
Updated Comment Value: some more comment strings
Whole Text: some text characters
Split text:  characters
Is Whitespace?: YES
