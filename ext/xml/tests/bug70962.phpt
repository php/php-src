--TEST--
Bug #70962 (XML_OPTION_SKIP_WHITE strips embedded whitespace)
--SKIPIF--
<?php
if (!extension_loaded('xml')) die('skip xml extension not available');
?>
--FILE--
<?php
function parseAndOutput($xml)
{
    $parser = xml_parser_create();
    xml_parser_set_option($parser, XML_OPTION_SKIP_WHITE, 1);

    xml_parse_into_struct($parser, $xml, $values);

    return $values;
}

$xml = "<a><b>&lt;d&gt;\n &lt;e&gt;</b><![CDATA[  ]]><c>\n \t</c></a>";

$parsed = parseAndOutput($xml);

// Check embedded whitespace is not getting skipped.
echo $parsed[1]['value'] . "\n";

// Check XML_OPTION_SKIP_WHITE ignores values of tags containing whitespace characters only.
var_dump(isset($parsed[2]['value']));

// Check XML_OPTION_SKIP_WHITE ignores empty <![CDATA[  ]]> values.
var_dump(count($parsed));

?>
--EXPECT--
<d>
 <e>
bool(false)
int(4)
