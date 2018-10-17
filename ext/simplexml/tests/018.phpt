--TEST--
SimpleXML: iteration through subnodes and attributes
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
$xml =<<<EOF
<people>
   <person name="Joe">
     Text1
     <child name="Ann" />
     Text2
     <child name="Marray" />
     Text3
   </person>
   <person name="Boe">
     <child name="Joe" />
     <child name="Ann" />
   </person>
</people>
EOF;
$xml1 =<<<EOF
<people>
   <person name="Joe">
     <child />
   </person>
</people>
EOF;

function traverse_xml($pad,$xml) {
  foreach($xml->children() as $name => $node) {
    echo $pad."<$name";
    foreach($node->attributes() as $attr => $value) {
      echo " $attr=\"$value\"";
    }
    echo ">\n";
    traverse_xml($pad."  ",$node);
    echo $pad."</$name>\n";
  }
}

traverse_xml("",simplexml_load_string($xml));
echo "----------\n";
traverse_xml("",simplexml_load_string($xml1));
echo "---Done---\n";
?>
--EXPECT--
<person name="Joe">
  <child name="Ann">
  </child>
  <child name="Marray">
  </child>
</person>
<person name="Boe">
  <child name="Joe">
  </child>
  <child name="Ann">
  </child>
</person>
----------
<person name="Joe">
  <child>
  </child>
</person>
---Done---
