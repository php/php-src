--TEST--
Bug #46048 (SimpleXML top-level @attributes not part of iterator)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php
$xml = '
<data id="1">
    <key>value</key>
</data>
';
$obj = simplexml_load_string($xml);
print_r(get_object_vars($obj));
?>
===DONE===
--EXPECT--	
Array
(
    [@attributes] => Array
        (
            [id] => 1
        )

    [key] => value
)
===DONE===
