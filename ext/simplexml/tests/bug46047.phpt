--TEST--
Bug #46047 (SimpleXML converts empty nodes into object with nested array)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
$xml = new SimpleXMLElement('<foo><bar><![CDATA[]]></bar><baz/></foo>',
  LIBXML_NOCDATA);
print_r($xml);

$xml = new SimpleXMLElement('<foo><bar></bar><baz/></foo>');
print_r($xml);

$xml = new SimpleXMLElement('<foo><bar/><baz/></foo>');
print_r($xml);
?>
--EXPECT--
SimpleXMLElement Object
(
    [bar] => SimpleXMLElement Object
        (
        )

    [baz] => SimpleXMLElement Object
        (
        )

)
SimpleXMLElement Object
(
    [bar] => SimpleXMLElement Object
        (
        )

    [baz] => SimpleXMLElement Object
        (
        )

)
SimpleXMLElement Object
(
    [bar] => SimpleXMLElement Object
        (
        )

    [baz] => SimpleXMLElement Object
        (
        )

)
	
