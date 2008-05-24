--TEST--
SPL: SimpleXMLIterator and recursion
--SKIPIF--
<?php 
if (!extension_loaded('simplexml')) print 'skip';
if (!extension_loaded("libxml")) print "skip LibXML not present";
?>
--FILE--
<?php 

$xml =<<<EOF
<?xml version='1.0'?>
<!DOCTYPE sxe SYSTEM "notfound.dtd">
<sxe id="elem1">
 Plain text.
 <elem1 attr1='first'>
  Bla bla 1.
  <!-- comment -->
  <elem2>
   Here we have some text data.
   <elem3>
    And here some more.
    <elem4>
     Wow once again.
    </elem4>
   </elem3>
  </elem2>
 </elem1>
 <elem11 attr2='second'>
  Bla bla 2.
  <elem111>
   Foo Bar
  </elem111>
 </elem11>
</sxe>
EOF;

$sxe = simplexml_load_string((binary)$xml, 'SimpleXMLIterator');

foreach(new RecursiveIteratorIterator($sxe, 1) as $name => $data) {
	var_dump($name);
	var_dump(get_class($data));
	var_dump(trim($data));
}

echo "===DUMP===\n";

var_dump(get_class($sxe));
var_dump(trim($sxe->elem1));

?>
===DONE===
--EXPECT--
string(5) "elem1"
string(17) "SimpleXMLIterator"
string(10) "Bla bla 1."
string(5) "elem2"
string(17) "SimpleXMLIterator"
string(28) "Here we have some text data."
string(5) "elem3"
string(17) "SimpleXMLIterator"
string(19) "And here some more."
string(5) "elem4"
string(17) "SimpleXMLIterator"
string(15) "Wow once again."
string(6) "elem11"
string(17) "SimpleXMLIterator"
string(10) "Bla bla 2."
string(7) "elem111"
string(17) "SimpleXMLIterator"
string(7) "Foo Bar"
===DUMP===
string(17) "SimpleXMLIterator"
string(10) "Bla bla 1."
===DONE===
--UEXPECT--
unicode(5) "elem1"
unicode(17) "SimpleXMLIterator"
unicode(10) "Bla bla 1."
unicode(5) "elem2"
unicode(17) "SimpleXMLIterator"
unicode(28) "Here we have some text data."
unicode(5) "elem3"
unicode(17) "SimpleXMLIterator"
unicode(19) "And here some more."
unicode(5) "elem4"
unicode(17) "SimpleXMLIterator"
unicode(15) "Wow once again."
unicode(6) "elem11"
unicode(17) "SimpleXMLIterator"
unicode(10) "Bla bla 2."
unicode(7) "elem111"
unicode(17) "SimpleXMLIterator"
unicode(7) "Foo Bar"
===DUMP===
unicode(17) "SimpleXMLIterator"
unicode(10) "Bla bla 1."
===DONE===
