--TEST--
SimpleXML and Entities
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$xml =<<<EOF
<?xml version='1.0'?>
<!DOCTYPE sxe SYSTEM "notfound.dtd" [
<!ENTITY included-entity "This is text included from an entity">
]>
<sxe id="elem1">
 Plain text.
 <elem1 attr1='first'>
  <!-- comment -->
  <elem2>
   <elem3>
    &included-entity;
    <elem4>
     <?test processing instruction ?>
    </elem4>
   </elem3>
  </elem2>
 </elem1>
</sxe>
EOF;

$sxe = simplexml_load_string($xml);

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
                            [included-entity] => SimpleXMLElement Object
                                (
                                    [included-entity] => This is text included from an entity
                                )

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
