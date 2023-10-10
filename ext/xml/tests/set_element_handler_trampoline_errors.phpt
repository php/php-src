--TEST--
Test xml_set_element_handler handlers as trampoline callback
--EXTENSIONS--
xml
--FILE--
<?php

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

/*
echo "1st arg is rubbish:\n";
$rc = new ReflectionClass(XMLParser::class);
$obj = $rc->newInstanceWithoutConstructor();

try {
    xml_set_element_handler($obj, $startCallback, $endCallback);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
*/
$parser = xml_parser_create();
/*
try {
    xml_set_element_handler($parser, $startCallback, $endCallback);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
*/
echo "2nd arg is rubbish:\n";
try {
    xml_set_element_handler($parser, [], $endCallback);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo "3rd arg is rubbish:\n";
try {
    xml_set_element_handler($parser, $startCallback, []);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
xml_parser_free($parser);

?>
--EXPECT--
2nd arg is rubbish:
TypeError: xml_set_element_handler(): Argument #2 ($start_handler) must be of type callable|string|null
3rd arg is rubbish:
TypeError: xml_set_element_handler(): Argument #2 ($start_handler) must be of type callable|string|null
