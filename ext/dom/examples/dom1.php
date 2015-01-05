<?php
require_once("dom1.inc");

echo "Test 1: accessing single nodes from php\n";
$dom = new domDocument;
$dom->loadxml($xmlstr);
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

// children() of of document would result in a memleak
//$children = $dom->children();
//print_node_list($children);

echo "--------- root\n";
$rootnode = $dom->documentElement;
print_node($rootnode);

echo "--------- children of root\n";
$children = $rootnode->childNodes;
print_node_list($children);

// The last node should be identical with the last entry in the children array
echo "--------- last\n";
$last = $rootnode->lastChild;
print_node($last);

// The parent of this last node is the root again
echo "--------- parent\n";
$parent = $last->parentNode;
print_node($parent);

// The children of this parent are the same children as one above
echo "--------- children of parent\n";
$children = $parent->childNodes;
print_node_list($children);

echo "--------- creating a new attribute\n";
//This is worthless
//$attr = $dom->createAttribute("src", "picture.gif");
//print_r($attr);

//$rootnode->set_attributeNode($attr); 
$attr = $rootnode->setAttribute("src", "picture.gif");
$attr = $rootnode->getAttribute("src");
print_r($attr);
print "\n";

echo "--------- Get Attribute Node\n";
$attr = $rootnode->getAttributeNode("src");
print_node($attr);

echo "--------- Remove Attribute Node\n";
$attr = $rootnode->removeAttribute("src");
print "Removed " . $attr . " attributes.\n";

echo "--------- attributes of rootnode\n";
$attrs = $rootnode->attributes;
print_node_list($attrs);

echo "--------- children of an attribute\n";
$children = $attrs->item(0)->childNodes;
print_node_list($children);

echo "--------- Add child to root\n";
$myelement = new domElement("Silly", "Symphony");
$newchild = $rootnode->appendChild($myelement);
print_node($newchild);
print $dom->saveXML();
print "\n";

echo "--------- Find element by tagname\n";
echo "    Using dom\n";
$children = $dom->getElementsByTagname("Silly");
print_node_list($children);

echo "    Using elem\n";
$children = $rootnode->getElementsByTagName("Silly");
print_node_list($children);

echo "--------- Unlink Node\n";
print_node($children->item(0));
$rootnode->removeChild($children->item(0));
print_node_list($rootnode->childNodes);
print $dom->savexml();

echo "--------- Find element by id\n";
print ("Not implemented\n");

echo "--------- Check various node_name return values\n";
print ("Not needed\n");

?>
