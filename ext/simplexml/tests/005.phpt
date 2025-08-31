--TEST--
SimpleXML: Text data
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
</sxe>
EOF
);

var_dump(trim($sxe->elem1->elem2));
var_dump(trim($sxe->elem1->elem2->elem3));
var_dump(trim($sxe->elem1->elem2->elem3->elem4));

echo "---Done---\n";

?>
--EXPECT--
string(28) "Here we have some text data."
string(19) "And here some more."
string(15) "Wow once again."
---Done--- 
