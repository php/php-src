--TEST--
Incorrectly initialized SimpleXmlElement
--EXTENSIONS--
simplexml
--FILE--
<?php

class MySXE extends SimpleXMLElement {
    public function __construct() {
        /* yolo */
    }
}

$sxe = new MySXE;
try {
    var_dump($sxe->count());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($sxe->xpath(''));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($sxe->getDocNamespaces());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($sxe->children());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($sxe->attributes());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($sxe->registerXPathNamespace('', ''));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($sxe->foo);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
SimpleXMLElement is not properly initialized
SimpleXMLElement is not properly initialized
SimpleXMLElement is not properly initialized
SimpleXMLElement is not properly initialized
SimpleXMLElement is not properly initialized
SimpleXMLElement is not properly initialized
SimpleXMLElement is not properly initialized
