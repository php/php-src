--TEST--
SimpleXML: iteration through subnodes
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
$xml =<<<EOF
<people>
   <person name="Joe">
     <child name="Ann" />
     <child name="Marray" />
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
     <child name="Ann" />
   </person>
</people>
EOF;

function print_xml($xml) {
  foreach($xml as $person) {
    echo "person: ".$person['name']."\n";
    foreach($person as $child) {
      echo "  child: ".$child['name']."\n";
    }
  }
  echo "----------\n";
}

function print_xml2($xml) {
  $persons = $xml->count("person");
  for ($i=0;$i<$persons;$i++) {
    echo "person: ".$xml->person[$i]['name']."\n";
    $children = $xml->person[$i]->count("child");
    for ($j=0;$j<$children;$j++) {
      echo "  child: ".$xml->person[$i]->child[$j]['name']."\n";
    }
  }
  echo "----------\n";
}

print_xml(simplexml_load_string($xml));
print_xml(simplexml_load_string($xml1));
print_xml2(simplexml_load_string($xml));
print_xml2(simplexml_load_string($xml1));
echo "---Done---\n";
?>
--EXPECT--
person: Joe
  child: Ann
  child: Marray
person: Boe
  child: Joe
  child: Ann
----------
person: Joe
  child: Ann
----------
person: Joe
  child: Ann
  child: Marray
person: Boe
  child: Joe
  child: Ann
----------
person: Joe
  child: Ann
----------
---Done---
