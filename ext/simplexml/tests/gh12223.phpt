--TEST--
GH-12223: Entity reference produces infinite loop in var_dump/print_r
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = <<<XML
<?xml version="1.0"?>
<!DOCTYPE somedoc [
  <!ENTITY a "something">
  <!ENTITY b "&a;">
  <!ENTITY c "&b;">
]>
<somedoc>&c;</somedoc>
XML;

$sxe = simplexml_load_string($xml);

var_dump($sxe);
print_r($sxe);

?>
--EXPECT--
object(SimpleXMLElement)#1 (1) {
  ["c"]=>
  object(SimpleXMLElement)#2 (1) {
    ["c"]=>
    object(SimpleXMLElement)#3 (1) {
      ["b"]=>
      object(SimpleXMLElement)#4 (1) {
        ["b"]=>
        object(SimpleXMLElement)#5 (1) {
          ["a"]=>
          object(SimpleXMLElement)#6 (1) {
            ["a"]=>
            string(9) "something"
          }
        }
      }
    }
  }
}
SimpleXMLElement Object
(
    [c] => SimpleXMLElement Object
        (
            [c] => SimpleXMLElement Object
                (
                    [b] => SimpleXMLElement Object
                        (
                            [b] => SimpleXMLElement Object
                                (
                                    [a] => SimpleXMLElement Object
                                        (
                                            [a] => something
                                        )

                                )

                        )

                )

        )

)
