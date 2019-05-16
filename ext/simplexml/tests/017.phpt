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
  foreach($xml->children() as $person) {
    echo "person: ".$person['name']."\n";
    foreach($person->children() as $child) {
      echo "  child: ".$child['name']."\n";
    }
  }
}

function print_xml2($xml) {
  $persons = 2;
  for ($i=0;$i<$persons;$i++) {
    echo "person: ".$xml->person[$i]['name']."\n";
	$children = 2;
    for ($j=0;$j<$children;$j++) {
      echo "  child: ".$xml->person[$i]->child[$j]['name']."\n";
    }
  }
}

echo "---11---\n";
print_xml(simplexml_load_string($xml));
echo "---12---\n";
print_xml(simplexml_load_string($xml1));
echo "---21---\n";
print_xml2(simplexml_load_string($xml));
echo "---22---\n";
print_xml2(simplexml_load_string($xml1));
?>
===DONE===
--EXPECTF--
---11---
person: Joe
  child: Ann
  child: Marray
person: Boe
  child: Joe
  child: Ann
---12---
person: Joe
  child: Ann
---21---
person: Joe
  child: Ann
  child: Marray
person: Boe
  child: Joe
  child: Ann
---22---
person: Joe
  child: Ann
  child: 
person: 

Notice: Trying to get property 'child' of non-object in %s017.php on line %d
  child: 

Notice: Trying to get property 'child' of non-object in %s017.php on line %d
  child: 
===DONE===
