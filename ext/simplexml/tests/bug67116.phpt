--TEST--
Bug #67116 (Inconsistent parsing of Nodes w/o linefeed)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<aa>
    <bs>
        <b>b</b>
    </bs>
    <cs><c>b</c></cs>
    <ds><d id="d"></d></ds>
    <es>
        <e id="e"></e>
    </es>
    <fs><f id="f"></f><f id="f"></f></fs>
</aa>
XML;
$sxe = simplexml_load_string($xml);
print_r($sxe);

?>
--EXPECT--
SimpleXMLElement Object
(
    [bs] => SimpleXMLElement Object
        (
            [b] => b
        )

    [cs] => SimpleXMLElement Object
        (
            [c] => b
        )

    [ds] => SimpleXMLElement Object
        (
            [d] => SimpleXMLElement Object
                (
                    [@attributes] => Array
                        (
                            [id] => d
                        )

                )

        )

    [es] => SimpleXMLElement Object
        (
            [e] => SimpleXMLElement Object
                (
                    [@attributes] => Array
                        (
                            [id] => e
                        )

                )

        )

    [fs] => SimpleXMLElement Object
        (
            [f] => Array
                (
                    [0] => SimpleXMLElement Object
                        (
                            [@attributes] => Array
                                (
                                    [id] => f
                                )

                        )

                    [1] => SimpleXMLElement Object
                        (
                            [@attributes] => Array
                                (
                                    [id] => f
                                )

                        )

                )

        )

)
