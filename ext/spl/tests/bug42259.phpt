--TEST--
Bug #42259 (SimpleXMLIterator loses ancestry)
--SKIPIF--
<?php
if (!extension_loaded('simplexml')) print 'skip';
if (!extension_loaded("libxml")) print "skip LibXML not present";
?>
--FILE--
<?php
$xml =<<<EOF
<xml>
  <fieldset1>
    <field1/>
    <field2/>
  </fieldset1>
  <fieldset2>
    <options>
      <option1/>
      <option2/>
      <option3/>
    </options>
    <field1/>
    <field2/>
  </fieldset2>
</xml>
EOF;

$sxe = new SimpleXMLIterator($xml);
$rit = new RecursiveIteratorIterator($sxe, RecursiveIteratorIterator::LEAVES_ONLY);
foreach ($rit as $child) {
  $path = '';
  $ancestry = $child->xpath('ancestor-or-self::*');
  foreach ($ancestry as $ancestor) {
    $path .= $ancestor->getName() . '/';
  }
  $path = substr($path, 0, strlen($path) - 1);
  echo count($ancestry) . ' steps: ' . $path . PHP_EOL;
}
?>
===DONE===
--EXPECT--
3 steps: xml/fieldset1/field1
3 steps: xml/fieldset1/field2
4 steps: xml/fieldset2/options/option1
4 steps: xml/fieldset2/options/option2
4 steps: xml/fieldset2/options/option3
3 steps: xml/fieldset2/field1
3 steps: xml/fieldset2/field2
===DONE===
