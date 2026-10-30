--TEST--
Tests for DOMProcessingInstruction class
--EXTENSIONS--
dom
--FILE--
<?php

class FailingStringable {
    public function __toString(): string {
        throw new Exception("failed in __toString");
    }
}

echo "--- Test construction ---\n";

try {
    $pi = new DOMProcessingInstruction("\0");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

$pi = new DOMProcessingInstruction("test");

echo "--- Test fields ---\n";

var_dump($pi->target);
var_dump($pi->data);
$pi->data = "ok";
var_dump($pi->data);
try {
    $pi->data = new FailingStringable;
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
var_dump($pi->data);
$pi->data = 12345;
var_dump($pi->data);
$pi->data = "my data <>";
var_dump($pi->data);

echo "--- Test appending ---\n";

$doc = new DOMDocument;
$doc->appendChild($doc->createElement('root'));
$doc->documentElement->appendChild($doc->adoptNode($pi));
echo $doc->saveXML();

echo "--- Test construction with __construct by reflection and fields ---\n";

$class = new ReflectionClass('DOMProcessingInstruction');
$instance = $class->newInstanceWithoutConstructor();

try {
    var_dump($instance->target);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($instance->data);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    $instance->data = "hello";
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
--- Test construction ---
Invalid Character Error
--- Test fields ---
string(4) "test"
string(0) ""
string(2) "ok"
failed in __toString
string(2) "ok"
string(5) "12345"
string(10) "my data <>"
--- Test appending ---
<?xml version="1.0"?>
<root><?test my data <>?></root>
--- Test construction with __construct by reflection and fields ---
Invalid State Error
Invalid State Error
Invalid State Error
