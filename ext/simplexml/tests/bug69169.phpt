--TEST--
Bug #69169 (simplexml_load_string parse wrongly when xml given in one row)
--SKIPIF--
<?php
if (!extension_loaded("simplexml")) die("skip SimpleXML not available");
?>
--FILE--
<?php
$a = '<?xml version="1.0" encoding="UTF-8"?>
<root a="b">
	<row b="y">
		<item s="t" />
	</row>
	<row p="c">
		<item y="n" />
	</row>
</root>';
$b = str_replace(array("\n", "\r", "\t"), "", $a);
$simple_xml = simplexml_load_string($b);
print_r($simple_xml);
?>
--EXPECT--
SimpleXMLElement Object
(
    [@attributes] => Array
        (
            [a] => b
        )

    [row] => Array
        (
            [0] => SimpleXMLElement Object
                (
                    [@attributes] => Array
                        (
                            [b] => y
                        )

                    [item] => SimpleXMLElement Object
                        (
                            [@attributes] => Array
                                (
                                    [s] => t
                                )

                        )

                )

            [1] => SimpleXMLElement Object
                (
                    [@attributes] => Array
                        (
                            [p] => c
                        )

                    [item] => SimpleXMLElement Object
                        (
                            [@attributes] => Array
                                (
                                    [y] => n
                                )

                        )

                )

        )

)
