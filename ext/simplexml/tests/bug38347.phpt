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

$xmlstr = "<xml><item>Item 1</item><item>Item 2</item></xml>";
$xml = simplexml_load_string($xmlstr);
iterate($xml->unknown);

echo "Done\n";
?>
--EXPECTF--
SimpleXMLElement Object
(
)

Warning: Invalid argument supplied for foreach() in %sbug38347.php on line 6
Done
