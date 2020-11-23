--TEST--
SimpleXML: Adding an elements
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
$xml =<<<EOF
<people></people>
EOF;

function traverse_xml($xml, $pad = '')
{
  $name = $xml->getName();
  echo "$pad<$name";
  foreach($xml->attributes() as $attr => $value)
  {
    echo " $attr=\"$value\"";
  }
  echo ">" . trim($xml) . "\n";
  foreach($xml->children() as $node)
  {
    traverse_xml($node, $pad.'  ');
  }
  echo $pad."</$name>\n";
}


$people = simplexml_load_string($xml);
traverse_xml($people);
$people->person = 'Joe';
$people->person['gender'] = 'male';
traverse_xml($people);
$people->person = 'Jane';
traverse_xml($people);
$people->person['gender'] = 'female';
$people->person[1] = 'Joe';
$people->person[1]['gender'] = 'male';
traverse_xml($people);
$people->person[3] = 'Minni-me';
$people->person[2]['gender'] = 'male';
traverse_xml($people);
$people->person[3]['gender'] = 'error';
traverse_xml($people);
?>
--EXPECTF--
<people>
</people>
<people>
  <person gender="male">Joe
  </person>
</people>
<people>
  <person gender="male">Jane
  </person>
</people>
<people>
  <person gender="female">Jane
  </person>
  <person gender="male">Joe
  </person>
</people>

Warning: main(): Cannot add element person number 3 when only 2 such elements exist in %s027.php on line %d
<people>
  <person gender="female">Jane
  </person>
  <person gender="male">Joe
  </person>
  <person gender="male">Minni-me
  </person>
</people>
<people>
  <person gender="female">Jane
  </person>
  <person gender="male">Joe
  </person>
  <person gender="male">Minni-me
  </person>
  <person gender="error">
  </person>
</people>
