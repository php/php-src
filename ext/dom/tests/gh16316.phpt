--TEST--
GH-16316 (DOMXPath breaks when not initialized properly)
--EXTENSIONS--
dom
--FILE--
<?php

class Demo extends DOMXPath {
    public function __construct() {}
}

$demo = new Demo;
try {
    var_dump($demo);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump($demo->document);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
object(Demo)#1 (1) {
  ["registerNodeNamespaces"]=>
  bool(true)
}
Invalid State Error
Invalid State Error
