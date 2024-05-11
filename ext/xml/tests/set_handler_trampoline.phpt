--TEST--
Test XMLParser generic handlers as trampoline callback
--EXTENSIONS--
xml
--FILE--
<?php
class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        echo 'Target: ', $arguments[1], PHP_EOL;
        echo 'Data: ', $arguments[2], PHP_EOL;
    }
}

$o = new TrampolineTest();
$callback = [$o, 'pi_handler'];

$xml = <<<HERE
<?xml version="1.0" encoding="ISO-8859-1"?>
<?xml-stylesheet href="default.xsl" type="text/xml"?>
HERE;

/* Use xml_set_processing_instruction_handler() for generic implementation */
$parser = xml_parser_create();
xml_set_processing_instruction_handler($parser, $callback);
xml_parse($parser, $xml, true);
xml_parser_free($parser);

?>
--EXPECT--
Trampoline for pi_handler
Target: xml-stylesheet
Data: href="default.xsl" type="text/xml"
