--TEST--
Bug #28721 (appendChild() and insertBefore() unset DOMText)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function print_node(DomNode $node) {
  echo "name (value): " . $node->nodeName . " (" . $node->nodeValue . ")\n";
}

function print_node_r(DomNode $node) {
  static $indent = "";
  echo "\n" . $indent;
  print_node($node);

  echo $indent . "parent: ";
  if ( $node->parentNode )
    print_node($node->parentNode);
  else
    echo "NULL\n";

  echo $indent . "previousSibling: ";
  if ( $node->previousSibling )
    print_node($node->previousSibling);
  else
    echo "NULL\n";
  
  echo $indent . "nextSibling: ";
  if ( $node->nextSibling )
    print_node($node->nextSibling);
  else
    echo "NULL\n";

  if ( !$node->hasChildNodes() )
    return;
  
  foreach ($node->childNodes as $child) {

    $old_indent  = $indent;
    $indent .= "  ";
    print_node_r($child);
    $indent = $old_indent;
  }
}

function err_handler($errno, $errstr, $errfile, $errline) {
  echo "Error ($errno) on line $errline: $errstr\n";
}

// Record 'DocumentFragment is empty' warnings
set_error_handler("err_handler", E_WARNING);

$xml = new DomDocument();

$p = $xml->createElement("p");

$p->appendChild($t1 = $xml->createTextNode(" t1 "));
$p->appendChild($b = $xml->createElement("b"));
$b->appendChild($xml->createTextNode("X"));
$p->appendChild($t2 = $xml->createTextNode(" t2 "));
$p->appendChild($xml->createTextNode(" xxx "));

print_node_r($p);

echo "\nAppend t1 to p:\n";
$ret = $p->appendChild($t1);

print_node_r($p);
echo "\n";

echo "t1 == ret: ";
var_dump( $t1 === $ret );


$d = $xml->createElement("div");
$d->appendChild($t3 = $xml->createTextNode(" t3 "));
$d->appendChild($b = $xml->createElement("b"));
$b->appendChild($xml->createElement("X"));
$d->appendChild($t4 = $xml->createTextNode(" t4 "));
$d->appendChild($xml->createTextNode(" xxx "));

echo "\ndiv:\n";
print_node_r($d);

echo "\nInsert t4 before t3:\n";

$ret = $d->insertBefore($t4, $t3);

print_node_r($d);
echo "\n";

$frag = $xml->createDocumentFragment();

$t5 = $frag->appendChild($xml->createTextNode(" t5 "));
$frag->appendChild($i = $xml->createElement("i"));
$i->appendChild($xml->createTextNode(" frob "));
$frag->appendChild($xml->createTextNOde(" t6 "));

echo "\np:\n";
print_node_r($p);
echo "\nFragment:\n";
print_node_r($frag);

echo "\nAppending fragment to p:\n";
$p->appendChild($frag);

print_node_r($p);
echo "\nFragment:\n";
print_node_r($frag);

echo "\ndiv:\n";
print_node_r($d);
echo "\nInserting fragment before t4\n";
$d->insertBefore($frag, $t4);
print_node_r($d);

echo "\np:\n";
print_node_r($p);

?>
--EXPECT--

name (value): p ( t1 X t2  xxx )
parent: NULL
previousSibling: NULL
nextSibling: NULL

  name (value): #text ( t1 )
  parent: name (value): p ( t1 X t2  xxx )
  previousSibling: NULL
  nextSibling: name (value): b (X)

  name (value): b (X)
  parent: name (value): p ( t1 X t2  xxx )
  previousSibling: name (value): #text ( t1 )
  nextSibling: name (value): #text ( t2 )

    name (value): #text (X)
    parent: name (value): b (X)
    previousSibling: NULL
    nextSibling: NULL

  name (value): #text ( t2 )
  parent: name (value): p ( t1 X t2  xxx )
  previousSibling: name (value): b (X)
  nextSibling: name (value): #text ( xxx )

  name (value): #text ( xxx )
  parent: name (value): p ( t1 X t2  xxx )
  previousSibling: name (value): #text ( t2 )
  nextSibling: NULL

Append t1 to p:

name (value): p (X t2  xxx  t1 )
parent: NULL
previousSibling: NULL
nextSibling: NULL

  name (value): b (X)
  parent: name (value): p (X t2  xxx  t1 )
  previousSibling: NULL
  nextSibling: name (value): #text ( t2 )

    name (value): #text (X)
    parent: name (value): b (X)
    previousSibling: NULL
    nextSibling: NULL

  name (value): #text ( t2 )
  parent: name (value): p (X t2  xxx  t1 )
  previousSibling: name (value): b (X)
  nextSibling: name (value): #text ( xxx )

  name (value): #text ( xxx )
  parent: name (value): p (X t2  xxx  t1 )
  previousSibling: name (value): #text ( t2 )
  nextSibling: name (value): #text ( t1 )

  name (value): #text ( t1 )
  parent: name (value): p (X t2  xxx  t1 )
  previousSibling: name (value): #text ( xxx )
  nextSibling: NULL

t1 == ret: bool(true)

div:

name (value): div ( t3  t4  xxx )
parent: NULL
previousSibling: NULL
nextSibling: NULL

  name (value): #text ( t3 )
  parent: name (value): div ( t3  t4  xxx )
  previousSibling: NULL
  nextSibling: name (value): b ()

  name (value): b ()
  parent: name (value): div ( t3  t4  xxx )
  previousSibling: name (value): #text ( t3 )
  nextSibling: name (value): #text ( t4 )

    name (value): X ()
    parent: name (value): b ()
    previousSibling: NULL
    nextSibling: NULL

  name (value): #text ( t4 )
  parent: name (value): div ( t3  t4  xxx )
  previousSibling: name (value): b ()
  nextSibling: name (value): #text ( xxx )

  name (value): #text ( xxx )
  parent: name (value): div ( t3  t4  xxx )
  previousSibling: name (value): #text ( t4 )
  nextSibling: NULL

Insert t4 before t3:

name (value): div ( t4  t3  xxx )
parent: NULL
previousSibling: NULL
nextSibling: NULL

  name (value): #text ( t4 )
  parent: name (value): div ( t4  t3  xxx )
  previousSibling: NULL
  nextSibling: name (value): #text ( t3 )

  name (value): #text ( t3 )
  parent: name (value): div ( t4  t3  xxx )
  previousSibling: name (value): #text ( t4 )
  nextSibling: name (value): b ()

  name (value): b ()
  parent: name (value): div ( t4  t3  xxx )
  previousSibling: name (value): #text ( t3 )
  nextSibling: name (value): #text ( xxx )

    name (value): X ()
    parent: name (value): b ()
    previousSibling: NULL
    nextSibling: NULL

  name (value): #text ( xxx )
  parent: name (value): div ( t4  t3  xxx )
  previousSibling: name (value): b ()
  nextSibling: NULL


p:

name (value): p (X t2  xxx  t1 )
parent: NULL
previousSibling: NULL
nextSibling: NULL

  name (value): b (X)
  parent: name (value): p (X t2  xxx  t1 )
  previousSibling: NULL
  nextSibling: name (value): #text ( t2 )

    name (value): #text (X)
    parent: name (value): b (X)
    previousSibling: NULL
    nextSibling: NULL

  name (value): #text ( t2 )
  parent: name (value): p (X t2  xxx  t1 )
  previousSibling: name (value): b (X)
  nextSibling: name (value): #text ( xxx )

  name (value): #text ( xxx )
  parent: name (value): p (X t2  xxx  t1 )
  previousSibling: name (value): #text ( t2 )
  nextSibling: name (value): #text ( t1 )

  name (value): #text ( t1 )
  parent: name (value): p (X t2  xxx  t1 )
  previousSibling: name (value): #text ( xxx )
  nextSibling: NULL

Fragment:

name (value): #document-fragment ()
parent: NULL
previousSibling: NULL
nextSibling: NULL

  name (value): #text ( t5 )
  parent: name (value): #document-fragment ()
  previousSibling: NULL
  nextSibling: name (value): i ( frob )

  name (value): i ( frob )
  parent: name (value): #document-fragment ()
  previousSibling: name (value): #text ( t5 )
  nextSibling: name (value): #text ( t6 )

    name (value): #text ( frob )
    parent: name (value): i ( frob )
    previousSibling: NULL
    nextSibling: NULL

  name (value): #text ( t6 )
  parent: name (value): #document-fragment ()
  previousSibling: name (value): i ( frob )
  nextSibling: NULL

Appending fragment to p:

name (value): p (X t2  xxx  t1  t5  frob  t6 )
parent: NULL
previousSibling: NULL
nextSibling: NULL

  name (value): b (X)
  parent: name (value): p (X t2  xxx  t1  t5  frob  t6 )
  previousSibling: NULL
  nextSibling: name (value): #text ( t2 )

    name (value): #text (X)
    parent: name (value): b (X)
    previousSibling: NULL
    nextSibling: NULL

  name (value): #text ( t2 )
  parent: name (value): p (X t2  xxx  t1  t5  frob  t6 )
  previousSibling: name (value): b (X)
  nextSibling: name (value): #text ( xxx )

  name (value): #text ( xxx )
  parent: name (value): p (X t2  xxx  t1  t5  frob  t6 )
  previousSibling: name (value): #text ( t2 )
  nextSibling: name (value): #text ( t1 )

  name (value): #text ( t1 )
  parent: name (value): p (X t2  xxx  t1  t5  frob  t6 )
  previousSibling: name (value): #text ( xxx )
  nextSibling: name (value): #text ( t5 )

  name (value): #text ( t5 )
  parent: name (value): p (X t2  xxx  t1  t5  frob  t6 )
  previousSibling: name (value): #text ( t1 )
  nextSibling: name (value): i ( frob )

  name (value): i ( frob )
  parent: name (value): p (X t2  xxx  t1  t5  frob  t6 )
  previousSibling: name (value): #text ( t5 )
  nextSibling: name (value): #text ( t6 )

    name (value): #text ( frob )
    parent: name (value): i ( frob )
    previousSibling: NULL
    nextSibling: NULL

  name (value): #text ( t6 )
  parent: name (value): p (X t2  xxx  t1  t5  frob  t6 )
  previousSibling: name (value): i ( frob )
  nextSibling: NULL

Fragment:

name (value): #document-fragment ()
parent: NULL
previousSibling: NULL
nextSibling: NULL

div:

name (value): div ( t4  t3  xxx )
parent: NULL
previousSibling: NULL
nextSibling: NULL

  name (value): #text ( t4 )
  parent: name (value): div ( t4  t3  xxx )
  previousSibling: NULL
  nextSibling: name (value): #text ( t3 )

  name (value): #text ( t3 )
  parent: name (value): div ( t4  t3  xxx )
  previousSibling: name (value): #text ( t4 )
  nextSibling: name (value): b ()

  name (value): b ()
  parent: name (value): div ( t4  t3  xxx )
  previousSibling: name (value): #text ( t3 )
  nextSibling: name (value): #text ( xxx )

    name (value): X ()
    parent: name (value): b ()
    previousSibling: NULL
    nextSibling: NULL

  name (value): #text ( xxx )
  parent: name (value): div ( t4  t3  xxx )
  previousSibling: name (value): b ()
  nextSibling: NULL

Inserting fragment before t4
Error (2) on line 109: DOMNode::insertBefore(): Document Fragment is empty

name (value): div ( t4  t3  xxx )
parent: NULL
previousSibling: NULL
nextSibling: NULL

  name (value): #text ( t4 )
  parent: name (value): div ( t4  t3  xxx )
  previousSibling: NULL
  nextSibling: name (value): #text ( t3 )

  name (value): #text ( t3 )
  parent: name (value): div ( t4  t3  xxx )
  previousSibling: name (value): #text ( t4 )
  nextSibling: name (value): b ()

  name (value): b ()
  parent: name (value): div ( t4  t3  xxx )
  previousSibling: name (value): #text ( t3 )
  nextSibling: name (value): #text ( xxx )

    name (value): X ()
    parent: name (value): b ()
    previousSibling: NULL
    nextSibling: NULL

  name (value): #text ( xxx )
  parent: name (value): div ( t4  t3  xxx )
  previousSibling: name (value): b ()
  nextSibling: NULL

p:

name (value): p (X t2  xxx  t1  t5  frob  t6 )
parent: NULL
previousSibling: NULL
nextSibling: NULL

  name (value): b (X)
  parent: name (value): p (X t2  xxx  t1  t5  frob  t6 )
  previousSibling: NULL
  nextSibling: name (value): #text ( t2 )

    name (value): #text (X)
    parent: name (value): b (X)
    previousSibling: NULL
    nextSibling: NULL

  name (value): #text ( t2 )
  parent: name (value): p (X t2  xxx  t1  t5  frob  t6 )
  previousSibling: name (value): b (X)
  nextSibling: name (value): #text ( xxx )

  name (value): #text ( xxx )
  parent: name (value): p (X t2  xxx  t1  t5  frob  t6 )
  previousSibling: name (value): #text ( t2 )
  nextSibling: name (value): #text ( t1 )

  name (value): #text ( t1 )
  parent: name (value): p (X t2  xxx  t1  t5  frob  t6 )
  previousSibling: name (value): #text ( xxx )
  nextSibling: name (value): #text ( t5 )

  name (value): #text ( t5 )
  parent: name (value): p (X t2  xxx  t1  t5  frob  t6 )
  previousSibling: name (value): #text ( t1 )
  nextSibling: name (value): i ( frob )

  name (value): i ( frob )
  parent: name (value): p (X t2  xxx  t1  t5  frob  t6 )
  previousSibling: name (value): #text ( t5 )
  nextSibling: name (value): #text ( t6 )

    name (value): #text ( frob )
    parent: name (value): i ( frob )
    previousSibling: NULL
    nextSibling: NULL

  name (value): #text ( t6 )
  parent: name (value): p (X t2  xxx  t1  t5  frob  t6 )
  previousSibling: name (value): i ( frob )
  nextSibling: NULL
