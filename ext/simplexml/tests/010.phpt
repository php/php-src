--TEST--
SimpleXML: Simple Inheritance
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

class simplexml_inherited extends SimpleXMLElement
{
}

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

$sxe = simplexml_load_string($xml, 'simplexml_inherited');

print_r($sxe);

?>
===DONE===
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
===DONE===
