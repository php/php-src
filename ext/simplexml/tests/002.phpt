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

$copy = $sxe->__clone();

print_r($copy);

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
