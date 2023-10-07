--TEST--
Swap underlying object to call methods with xml_set_object()
--EXTENSIONS--
xml
--FILE--
<?php

class A {
    public function start_element($parser, $name, $attributes) {
        global $b;
        xml_set_object($parser, $b);
        echo "A::start_element($name)\n";
    }
}

class B {
    public function start_element($parser, $name) {
        echo "B::start_element($name)\n";
    }
}

$a = new A;
$b = new B;

$parser = xml_parser_create();
xml_set_object($parser, $a);
xml_set_element_handler($parser, "start_element", null);
xml_parse($parser, <<<XML
<?xml version="1.0"?>
<container>
    <child>
</container>
XML);

?>
--EXPECT--
A::start_element(CONTAINER)
B::start_element(CHILD)
