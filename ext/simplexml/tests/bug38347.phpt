--TEST--
Bug #38347 (Segmentation fault when using foreach with an unknown/empty SimpleXMLElement)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

function iterate($xml)
{
    print_r($xml);
    try {
        foreach ($xml->item as $item) {
            echo "This code will crash!";
        }
    } catch (\TypeError $e) {
        echo $e->getMessage(), \PHP_EOL;
    }
}

$xmlstr = "<xml><item>Item 1</item><item>Item 2</item></xml>";
$xml = simplexml_load_string($xmlstr);
iterate($xml->unknown);

echo "Done\n";
?>
--EXPECT--
SimpleXMLElement Object
(
)
foreach() argument must be of type array|object, null given
Done
