--TEST--
Bug #38347 (Segmentation fault when using foreach with an unknown/empty SimpleXMLElement)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

function iterate($xml)
{
    print_r($xml);
    foreach ($xml->item as $item) {
        echo "This code will crash!";
    }
}

$xmlstr = b"<xml><item>Item 1</item><item>Item 2</item></xml>";
$xml = simplexml_load_string($xmlstr);
iterate($xml->unknown);

echo "Done\n";
?>
--EXPECTF--	
SimpleXMLElement Object
(
)

Warning: iterate(): Node no longer exists in %s on line %d
Done
--UEXPECTF--
SimpleXMLElement Object
(
)

Warning: iterate(): Node no longer exists in %s on line %d
Done
