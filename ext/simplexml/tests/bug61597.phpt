--TEST--
Bug #61597 (SXE properties may lack attributes and content)
--SKIPIF--
<?php
if (!extension_loaded('simplexml')) die('skip simplexml extension not available');
?>
--FILE--
<?php
$xml = <<<'EOX'
<?xml version="1.0"?>
<data>
<datum file-key="8708124062829849862">corn</datum>
</data>
EOX;

var_dump(simplexml_load_string($xml));
?>
--EXPECTF--
object(SimpleXMLElement)#%d (1) {
  ["datum"]=>
  object(SimpleXMLElement)#%d (2) {
    ["@attributes"]=>
    array(1) {
      ["file-key"]=>
      string(19) "8708124062829849862"
    }
    [0]=>
    string(4) "corn"
  }
}
