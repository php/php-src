--TEST--
Swap underlying object to call methods with xml_set_object()
--EXTENSIONS--
xml
--FILE--
<?php

function end_handler(XMLParser $parser, string $tag) {
    echo "end_handler($tag)\n";
}

class A {
    public function start_element($parser, $name, $attributes) {
        global $b;
        xml_set_object($parser, $b);
        echo "A::start_element($name)\n";
    }
    public function PIHandler($parser, $target, $data) {
        echo "A::PIHandler($target)\n";
    }
}

class B {
    public function start_element($parser, $name) {
        echo "B::start_element($name)\n";
    }
    public function end_element($parser, $name) {
        echo "B::end_element($name)\n";
    }
    public function PIHandler($parser, $target, $data) {
        echo "B::PIHandler($target)\n";
    }
}

$a = new A;
$b = new B;

$parser = xml_parser_create();
xml_set_object($parser, $a);
xml_set_element_handler($parser, "start_element", "end_handler");
xml_set_processing_instruction_handler($parser, [$a, "PIHandler"]);
xml_parse($parser, <<<XML
<?xml version="1.0"?>
<container>
    <child/>
</container>
<?pi-test data ?>
XML);

?>
--EXPECT--
A::start_element(CONTAINER)
B::start_element(CHILD)
end_handler(CHILD)
end_handler(CONTAINER)
A::PIHandler(pi-test)
