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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($sxe->xpath(''));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($sxe->getDocNamespaces());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($sxe->children());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($sxe->attributes());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($sxe->registerXPathNamespace('', ''));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump($sxe->foo);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: SimpleXMLElement is not properly initialized
Error: SimpleXMLElement is not properly initialized
Error: SimpleXMLElement is not properly initialized
Error: SimpleXMLElement is not properly initialized
Error: SimpleXMLElement is not properly initialized
Error: SimpleXMLElement is not properly initialized
Error: SimpleXMLElement is not properly initialized
