--TEST--
SimpleXML: isset and unset by offset
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
$xml =<<<EOF
<root s:att1="b" att1="a"
      xmlns:s="urn::test" xmlns:t="urn::test-t">
   <child1>test</child1>
   <child1>test 2</child1>
   <s:child3 />
</root>
EOF;

$sxe = simplexml_load_string($xml);

echo $sxe->child1[0]."\n";
echo $sxe->child1[1]."\n\n";

var_dump(isset($sxe->child1[1]));
unset($sxe->child1[1]);
var_dump(isset($sxe->child1[1]));
echo "\n";

$atts = $sxe->attributes("urn::test");
var_dump(isset($atts[0]));
unset($atts[0]);
var_dump(isset($atts[0]));
var_dump(isset($atts[TRUE]));

?>
--EXPECT--
test
test 2

bool(true)
bool(false)

bool(true)
bool(false)
bool(false)
