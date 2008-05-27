--TEST--
SimpleXML: foreach with children()
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$sxe = simplexml_load_string(b<<<EOF
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
EOF
);

foreach($sxe->children() as $name => $data) {
	var_dump($name);
	var_dump(trim($data));
}

echo "===CLONE===\n";

foreach(clone $sxe->children() as $name => $data) {
	var_dump($name);
	var_dump(trim($data));
}

echo "===ELEMENT===\n";

foreach($sxe->elem11->children() as $name => $data) {
	var_dump($name);
	var_dump(trim($data));
}

echo "===COMMENT===\n";

foreach($sxe->elem1->children() as $name => $data) {
	var_dump($name);
	var_dump(trim($data));
}

?>
===DONE===
--EXPECT--
unicode(5) "elem1"
unicode(10) "Bla bla 1."
unicode(6) "elem11"
unicode(10) "Bla bla 2."
===CLONE===
unicode(5) "elem1"
unicode(10) "Bla bla 1."
unicode(6) "elem11"
unicode(10) "Bla bla 2."
===ELEMENT===
unicode(7) "elem111"
unicode(7) "Foo Bar"
===COMMENT===
unicode(5) "elem2"
unicode(28) "Here we have some text data."
===DONE===
