<?php
$xmls = array(
    '<?xml version="1.0"?><element>',
    '<?xml>',
    '<?xml version="dummy">',
    '<?xml?>',
    '<?xml version="1.0"?><elem></element>',
);

foreach ($xmls as $xml) {
    $xml_parser = xml_parser_create();
    if (!xml_parse($xml_parser, $xml, true)) {
        var_dump(xml_get_error_code($xml_parser));
        var_dump(xml_error_string(xml_get_error_code($xml_parser)));
    }
    xml_parser_free($xml_parser);
}