--TEST--
XML parser case folding test
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
chdir(dirname(__FILE__));

$xp = xml_parser_create();
xml_parser_set_option($xp, XML_OPTION_CASE_FOLDING, false);
xml_set_element_handler($xp, "start_element", "end_element");
$fp = fopen("xmltest.xml", "r");
while ($data = fread($fp, 4096)) {
	xml_parse($xp, $data, feof($fp));
}
xml_parser_free($xp);
$xp = xml_parser_create();
xml_parser_set_option($xp, XML_OPTION_CASE_FOLDING, true);
xml_set_element_handler($xp, "start_element", "end_element");
$fp = fopen("xmltest.xml", "r");
while ($data = fread($fp, 4096)) {
	xml_parse($xp, $data, feof($fp));
}
xml_parser_free($xp);

function start_element($xp, $elem, $attribs)
{
	print "<$elem";
	if (sizeof($attribs)) {
		while (list($k, $v) = each($attribs)) {
			print " $k=\"$v\"";
		}
	}
	print ">\n";
}

function end_element($xp, $elem)
{
	print "</$elem>\n";
}
?>
--EXPECT--
<root id="elem1">
<elem1>
<elem2>
<elem3>
<elem4>
</elem4>
</elem3>
</elem2>
</elem1>
</root>
<ROOT ID="elem1">
<ELEM1>
<ELEM2>
<ELEM3>
<ELEM4>
</ELEM4>
</ELEM3>
</ELEM2>
</ELEM1>
</ROOT>
