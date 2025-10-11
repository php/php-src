--TEST--
GH-16317 (SimpleXML does not allow __debugInfo() overrides to work)
--FILE--
<?php

class MySXE extends SimpleXMLElement {
    public function __debugInfo(): array {
        echo "invoked\n";
        return ['x' => 1];
    }
}

$sxe = simplexml_load_string('<root><a/></root>', MySXE::class);
var_dump($sxe);

?>
--EXPECT--
invoked
object(MySXE)#1 (1) {
  ["x"]=>
  int(1)
}
