--TEST--
SimpleXML and CDATA
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$sxe = simplexml_load_file(dirname(__FILE__).'/004.xml');

print_r($sxe);

echo "---Done---\n";

?>
--EXPECT--
simplexml_element Object
(
    [elem1] => simplexml_element Object
        (
            [comment] => simplexml_element Object
                (
                )

            [elem2] => simplexml_element Object
                (
                    [CDATA] => simplexml_element Object
                        (
                        )

                    [elem3] => simplexml_element Object
                        (
                            [elem4] => simplexml_element Object
                                (
                                    [test] => simplexml_element Object
                                        (
                                        )

                                )

                        )

                )

        )

)
---Done--- 
