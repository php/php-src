--TEST--
SimpleXMLElement::count() interaction with ReturnTypeWillChange
--EXTENSIONS--
simplexml
--FILE--
<?php

class CustomClass extends SimpleXMLElement {
    #[\ReturnTypeWillChange]
    public function count(): string {
        return "3";
    }
}

$sxe = simplexml_load_string("<root/>", CustomClass::class);
var_dump(count($sxe));

?>
--EXPECT--
int(3)
