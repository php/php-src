--TEST--
SPL: SimpleXMLIterator
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
<?php if (!extension_loaded("simplexml")) print "skip SimpleXML not present"; ?>
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

$sxe = simplexml_load_string($xml, 'SimpleXMLIterator');

print_r($sxe);

?>
===DONE===
--EXPECT--
SimpleXMLIterator Object
(
    [elem1] => SimpleXMLIterator Object
        (
            [comment] => SimpleXMLIterator Object
                (
                )

            [elem2] => SimpleXMLIterator Object
                (
                    [elem3] => SimpleXMLIterator Object
                        (
                            [elem4] => SimpleXMLIterator Object
                                (
                                    [test] => SimpleXMLIterator Object
                                        (
                                        )

                                )

                        )

                )

        )

)
===DONE===
