--TEST--
SimpleXML: Property assignment return value
--EXTENSIONS--
simplexml
--FILE--
<?php
$xml =<<<EOF
<root></root>
EOF;
$root = simplexml_load_string($xml);
var_dump($root->prop = 42);
?>
--EXPECT--
int(42)
