--TEST--
SimpleXML: foreach
--EXTENSIONS--
simplexml
--FILE--
<?php
$sxe = simplexml_load_string(<<<EOF
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
 </elem11>
</sxe>
EOF
);
foreach($sxe->children() as $name=>$val) {
    var_dump($name);
    var_dump(get_class($val));
    var_dump(trim($val));
}
?>
--EXPECT--
string(5) "elem1"
string(16) "SimpleXMLElement"
string(10) "Bla bla 1."
string(6) "elem11"
string(16) "SimpleXMLElement"
string(10) "Bla bla 2."
