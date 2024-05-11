--TEST--
Test xml_set_element_handler handlers as trampoline callback
--EXTENSIONS--
xml
--FILE--
<?php

class CustomXmlParser
{
    public function startHandler($XmlParser, $tag, $attr)
    {
        echo 'Method start handler: ', $tag, PHP_EOL;
    }

    public function endHandler($XmlParser, $tag)
    {
        echo 'Method end handler: ', $tag, PHP_EOL;
    }
}

$customParser = new CustomXmlParser;

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        echo 'Tag: ', $arguments[1], PHP_EOL;
    }
}

$o = new TrampolineTest();
$startCallback = [$o, 'start_handler'];
$endCallback = [$o, 'end_handler'];

$xml = <<<HERE
<a>
  <b/>
  <c>Text</c>
</a>
HERE;

echo "Both handlers are trampolines:\n";
$parser = xml_parser_create();
xml_set_element_handler($parser, $startCallback, $endCallback);
xml_parse($parser, $xml, true);
xml_parser_free($parser);

echo "\nStart handler is trampoline, end handler method string:\n";
$parser = xml_parser_create();
xml_set_object($parser, $customParser);
xml_set_element_handler($parser, $startCallback, 'endHandler');
xml_parse($parser, $xml, true);
xml_parser_free($parser);

echo "\nEnd handler is trampoline, start handler method string:\n";
$parser = xml_parser_create();
xml_set_object($parser, $customParser);
xml_set_element_handler($parser, 'startHandler', $endCallback);
xml_parse($parser, $xml, true);
xml_parser_free($parser);

?>
--EXPECT--
Both handlers are trampolines:
Trampoline for start_handler
Tag: A
Trampoline for start_handler
Tag: B
Trampoline for end_handler
Tag: B
Trampoline for start_handler
Tag: C
Trampoline for end_handler
Tag: C
Trampoline for end_handler
Tag: A

Start handler is trampoline, end handler method string:
Trampoline for start_handler
Tag: A
Trampoline for start_handler
Tag: B
Method end handler: B
Trampoline for start_handler
Tag: C
Method end handler: C
Method end handler: A

End handler is trampoline, start handler method string:
Method start handler: A
Method start handler: B
Trampoline for end_handler
Tag: B
Method start handler: C
Trampoline for end_handler
Tag: C
Trampoline for end_handler
Tag: A
