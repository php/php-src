--TEST--
GH-23818 (Changing XML handlers while parsing is rejected)
--EXTENSIONS--
xml
--FILE--
<?php

$startHandler = static function (XMLParser $parser): void {
    $replacement = static function () {};

    try {
        xml_set_element_handler($parser, $replacement, $replacement);
    } catch (Error $error) {
        echo 'xml_set_element_handler: ', $error->getMessage(), "\n";
    }

    try {
        xml_set_character_data_handler($parser, $replacement);
    } catch (Error $error) {
        echo 'xml_set_character_data_handler: ', $error->getMessage(), "\n";
    }
};

$parser = xml_parser_create();
xml_set_element_handler($parser, $startHandler, static function () {
    echo "end\n";
});
xml_set_character_data_handler($parser, static function (XMLParser $parser, string $data) {
    echo $data, "\n";
});

var_dump(xml_parse($parser, '<root>text</root>', true));
var_dump(xml_set_element_handler($parser, null, null));
var_dump(xml_set_character_data_handler($parser, null));
?>
--EXPECT--
xml_set_element_handler: Cannot change handlers while parsing
xml_set_character_data_handler: Cannot change handlers while parsing
text
end
int(1)
bool(true)
bool(true)
