--TEST--
SimpleXML: Simple document
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$sxe = simplexml_load_file(dirname(__FILE__).'/sxe.xml');

print_r($sxe);

echo "---Done---\n";

?>
--EXPECT--
SimpleXMLElement Object
(
    [elem1] => SimpleXMLElement Object
        (
            [comment] => SimpleXMLElement Object
                (
                )

            [elem2] => SimpleXMLElement Object
                (
                    [elem3] => SimpleXMLElement Object
                        (
                            [elem4] => SimpleXMLElement Object
                                (
                                    [test] => SimpleXMLElement Object
                                        (
                                        )

                                )

                        )

                )

        )

)
---Done--- 
