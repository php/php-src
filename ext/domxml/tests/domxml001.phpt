--TEST--
Test 1: Accessing single node
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("domxml_test.inc");

echo "Test 1: accessing single nodes from php\n";
$dom = xmldoc($xmlstr);
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

// children() of of document would result in a memleak
//$children = $dom->children();
//print_node_list($children);

echo "--------- root\n";
$rootnode = $dom->root();
print_node($rootnode);

echo "--------- children of root\n";
$children = $rootnode->children();
print_node_list($children);

// The last node should be identical with the last entry in the children array
echo "--------- last\n";
$last = $rootnode->last_child();
print_node($last);

// The parent of this last node is the root again
echo "--------- parent\n";
$parent = $last->parent();
print_node($parent);

// The children of this parent are the same children as one above
echo "--------- children of parent\n";
$children = $parent->children();
print_node_list($children);

echo "--------- creating a new attribute\n";
//This is worthless
//$attr = $dom->create_attribute("src", "picture.gif");
//print_r($attr);

//$rootnode->set_attribute_node($attr); /* Not implemented */
$attr = $rootnode->set_attribute("src", "picture.gif");
$attr = $rootnode->get_attribute("src");
print_r($attr);
print "\n";

echo "--------- Get Attribute Node\n";
$attr = $rootnode->get_attribute_node("src");
print_node($attr);

echo "--------- Remove Attribute Node\n";
$attr = $rootnode->remove_attribute("src");
print "Removed " . $attr . " attributes.\n";

echo "--------- attributes of rootnode\n";
$attrs = $rootnode->attributes();
print_node_list($attrs);

echo "--------- children of an attribute\n";
$children = $attrs[0]->children();
print_node_list($children);

echo "--------- Add child to root\n";
$newchild = $rootnode->new_child("Silly", "Symphony");
print_node($newchild);
print $dom->dumpmem();
print "\n";

echo "--------- Find element by tagname\n";
echo "    Using dom\n";
$children = $dom->get_elements_by_tagname("Silly");
print_node_list($children);
echo "    Using elem\n";
$children = $rootnode->get_elements_by_tagname("Silly");
print_node_list($children);

echo "--------- Unlink Node\n";
print_node($children[0]);
//domxml_node_unlink_node($children[0]);
$children[0]->unlink_node();
print_node_list($rootnode->children());
print $dom->dumpmem();

echo "--------- Find element by id\n";
print ("Not implemented\n");

echo "--------- Check various node_name return values\n";
print ("Not needed\n");

?>
--EXPECT--
Test 1: accessing single nodes from php
--------- root
Node Name: chapter
Node Type: 1
Num Children: 4

--------- children of root
Node Name: title
Node Type: 1
Num Children: 1
Node Content: Title

Node Name: #text
Node Type: 3
Num Children: 0
Node Content: 


Node Name: para
Node Type: 1
Num Children: 7

Node Name: #text
Node Type: 3
Num Children: 0
Node Content: 


--------- last
Node Name: #text
Node Type: 3
Num Children: 0
Node Content: 


--------- parent
Node Name: chapter
Node Type: 1
Num Children: 4

--------- children of parent
Node Name: title
Node Type: 1
Num Children: 1
Node Content: Title

Node Name: #text
Node Type: 3
Num Children: 0
Node Content: 


Node Name: para
Node Type: 1
Num Children: 7

Node Name: #text
Node Type: 3
Num Children: 0
Node Content: 


--------- creating a new attribute
picture.gif
--------- Get Attribute Node
Node Name: src
Node Type: 2
Num Children: 1
Node Content: picture.gif

--------- Remove Attribute Node
Removed 1 attributes.
--------- attributes of rootnode
Node Name: language
Node Type: 2
Num Children: 1
Node Content: en

--------- children of an attribute
Node Name: #text
Node Type: 3
Num Children: 0
Node Content: en

--------- Add child to root
Node Name: Silly
Node Type: 1
Num Children: 1
Node Content: Symphony

<?xml version="1.0" standalone="yes"?>
<!DOCTYPE chapter SYSTEM "/share/sgml/Norman_Walsh/db3xml10/db3xml10.dtd" [
<!ENTITY sp "spanish">
]>
<!-- lsfj  -->
<chapter language="en"><title language="en">Title</title>
<para language="ge">
&sp;
<!-- comment -->
<informaltable language="&sp;kkk">
<tgroup cols="3">
<tbody>
<row><entry>a1</entry><entry morerows="1">b1</entry><entry>c1</entry></row>
<row><entry>a2</entry><entry>c2</entry></row>
<row><entry>a3</entry><entry>b3</entry><entry>c3</entry></row>
</tbody>
</tgroup>
</informaltable>
</para>
<Silly>Symphony</Silly></chapter>

--------- Find element by tagname
    Using dom
Node Name: Silly
Node Type: 1
Num Children: 1
Node Content: Symphony

    Using elem
Node Name: Silly
Node Type: 1
Num Children: 1
Node Content: Symphony

--------- Unlink Node
Node Name: Silly
Node Type: 1
Num Children: 1
Node Content: Symphony

Node Name: title
Node Type: 1
Num Children: 1
Node Content: Title

Node Name: #text
Node Type: 3
Num Children: 0
Node Content: 


Node Name: para
Node Type: 1
Num Children: 7

Node Name: #text
Node Type: 3
Num Children: 0
Node Content: 


<?xml version="1.0" standalone="yes"?>
<!DOCTYPE chapter SYSTEM "/share/sgml/Norman_Walsh/db3xml10/db3xml10.dtd" [
<!ENTITY sp "spanish">
]>
<!-- lsfj  -->
<chapter language="en"><title language="en">Title</title>
<para language="ge">
&sp;
<!-- comment -->
<informaltable language="&sp;kkk">
<tgroup cols="3">
<tbody>
<row><entry>a1</entry><entry morerows="1">b1</entry><entry>c1</entry></row>
<row><entry>a2</entry><entry>c2</entry></row>
<row><entry>a3</entry><entry>b3</entry><entry>c3</entry></row>
</tbody>
</tgroup>
</informaltable>
</para>
</chapter>
--------- Find element by id
Not implemented
--------- Check various node_name return values
Not needed
