--TEST--
Bug #66084 simplexml_load_string() mangles empty node name, json variant
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip simplexml not available"; ?>
<?php if (!extension_loaded("json")) print "skip json not available"; ?>
--FILE--
<?php
echo json_encode(simplexml_load_string('<a><b/><c><x/></c></a>')), "\n";
echo json_encode(simplexml_load_string('<a><b/><d/><c><x/></c></a>')), "\n";
echo json_encode(simplexml_load_string('<a><b/><c><d/><x/></c></a>')), "\n";
echo json_encode(simplexml_load_string('<a><b/><c><d><x/></d></c></a>')), "\n";
?>
--EXPECT--
{"b":{},"c":{"x":{}}}
{"b":{},"d":{},"c":{"x":{}}}
{"b":{},"c":{"d":{},"x":{}}}
{"b":{},"c":{"d":{"x":{}}}}
