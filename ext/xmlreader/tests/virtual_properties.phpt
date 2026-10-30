--TEST--
Virtual property tests
--EXTENSIONS--
xmlreader
--FILE--
<?php

$rc = new ReflectionClass(XMLReader::class);
$prop = $rc->getProperty("nodeType");
var_dump($prop->isVirtual());
var_dump($prop->getSettableType());
var_dump($prop->getHooks());
var_dump($prop->getRawValue(new XMLReader));
var_dump($prop->getValue(new XMLReader));

$reader = XMLReader::XML("<root>hi</root>");
var_dump(json_encode($reader));
var_export($reader); echo "\n";
var_dump(get_object_vars($reader));

?>
--EXPECTF--
bool(true)
object(ReflectionNamedType)#%d (0) {
}
array(0) {
}
int(0)
int(0)
string(2) "{}"
\XMLReader::__set_state(array(
))
array(0) {
}
