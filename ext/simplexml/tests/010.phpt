--TEST--
SimpleXML: Simple Inheritance
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

class simplexml_inherited extends simplexml_element
{
}

$sxe = simplexml_load_file(dirname(__FILE__).'/sxe.xml', 'simplexml_inherited');

print_r($sxe);

echo "---Done---\n";

?>
--EXPECT--
simplexml_inherited Object
(
    [elem1] => simplexml_inherited Object
        (
            [comment] => simplexml_inherited Object
                (
                )

            [elem2] => simplexml_inherited Object
                (
                    [elem3] => simplexml_inherited Object
                        (
                            [elem4] => simplexml_inherited Object
                                (
                                    [test] => simplexml_inherited Object
                                        (
                                        )

                                )

                        )

                )

        )

)
---Done--- 
