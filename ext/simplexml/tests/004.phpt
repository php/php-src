--TEST--
SimpleXML and CDATA
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php 

$sxe = simplexml_load_string(<<<EOF
<?xml version='1.0'?>
<!DOCTYPE sxe SYSTEM "notfound.dtd">
<sxe id="elem1">
 Plain text.
 <elem1 attr1='first'>
  <!-- comment -->
  <elem2>
   <![CDATA[CDATA block]]>
   <elem3>
    <elem4>
     <?test processing instruction ?>
    </elem4>
   </elem3>
  </elem2>
 </elem1>
</sxe>
EOF
);

print_r($sxe);

$elem1 = $sxe->elem1;
$elem2 = $elem1->elem2;
var_dump(trim((string)$elem2));

?>
===DONE===
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
string(11) "CDATA block"
===DONE===
