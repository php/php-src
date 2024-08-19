--TEST--
Unsetting a handler via an empty string should emit a deprecation
--EXTENSIONS--
xml
--FILE--
<?php

/* Use xml_set_processing_instruction_handler() for generic implementation */
function dummy() {}

/* Create valid parser */
$parser = xml_parser_create();
xml_set_processing_instruction_handler($parser, dummy(...));

try {
    xml_set_processing_instruction_handler($parser, '');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
END
--EXPECTF--
Deprecated: xml_set_processing_instruction_handler(): Passing non-callable strings is deprecated since 8.4 in %s on line %d
END
