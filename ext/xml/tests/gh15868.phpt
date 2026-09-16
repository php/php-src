--TEST--
GH-15868 (Assertion failure in xml_parse_into_struct after exception)
--EXTENSIONS--
xml
--FILE--
<?php
$parser = xml_parser_create();
xml_set_element_handler($parser,
    function ($parser, $name, $attrs) {
        throw new Error('stop 1');
    }, function ($parser, $name) {
    }
);
try {
    xml_parse_into_struct($parser, "<container/>", $values, $tags);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$parser = xml_parser_create();
xml_set_element_handler($parser,
    function ($parser, $name, $attrs) {
    }, function ($parser, $name) {
        throw new Error('stop 2');
    }
);
try {
    xml_parse_into_struct($parser, "<container/>", $values, $tags);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$parser = xml_parser_create();
xml_set_character_data_handler($parser, function() {
    throw new Error('stop 3');
});
try {
    xml_parse_into_struct($parser, "<root><![CDATA[x]]></root>", $values, $tags);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: stop 1
Error: stop 2
Error: stop 3
