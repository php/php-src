--TEST--
SimpleXML and clone
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$xml =<<<EOF
<?xml version='1.0'?>
<!DOCTYPE sxe SYSTEM "notfound.dtd">
<sxe id="elem1">
 <elem1 attr1='first'>
  <!-- comment -->
  <elem2>
   <elem3>
    <elem4>
     <?test processing instruction ?>
    </elem4>
   </elem3>
  </elem2>
 </elem1>
</sxe>
EOF;

$sxe = simplexml_load_string($xml);

$copy = clone $sxe;

print_r($copy);

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
