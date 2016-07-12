--TEST--
json_encode() & extended encoding
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
$a = array('<foo>', "'bar'", '"baz"', '&blong&');

echo "Normal: ", json_encode($a), "\n";
echo "Tags: ",   json_encode($a, JSON_HEX_TAG), "\n";
echo "Apos: ",   json_encode($a, JSON_HEX_APOS), "\n";
echo "Quot: ",   json_encode($a, JSON_HEX_QUOT), "\n";
echo "Amp: ",    json_encode($a, JSON_HEX_AMP), "\n";
echo "All: ",    json_encode($a, JSON_HEX_TAG|JSON_HEX_APOS|JSON_HEX_QUOT|JSON_HEX_AMP), "\n";
?>
--EXPECT--
Normal: ["<foo>","'bar'","\"baz\"","&blong&"]
Tags: ["\u003Cfoo\u003E","'bar'","\"baz\"","&blong&"]
Apos: ["<foo>","\u0027bar\u0027","\"baz\"","&blong&"]
Quot: ["<foo>","'bar'","\u0022baz\u0022","&blong&"]
Amp: ["<foo>","'bar'","\"baz\"","\u0026blong\u0026"]
All: ["\u003Cfoo\u003E","\u0027bar\u0027","\u0022baz\u0022","\u0026blong\u0026"]
