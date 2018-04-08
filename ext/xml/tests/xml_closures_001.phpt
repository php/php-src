--TEST--
XML parser test using closures as callbacks
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
chdir(dirname(__FILE__));

$start_element = function ($xp, $elem, $attribs)
{
	print "<$elem";
	if (sizeof($attribs)) {
        foreach ($attribs as $k => $v) {
			print " $k=\"$v\"";
		}
	}
	print ">\n";
};

$end_element = function ($xp, $elem)
{
	print "</$elem>\n";
};

$xp = xml_parser_create();
xml_parser_set_option($xp, XML_OPTION_CASE_FOLDING, false);
xml_set_element_handler($xp, $start_element, $end_element);
$fp = fopen("xmltest.xml", "r");
while ($data = fread($fp, 4096)) {
	xml_parse($xp, $data, feof($fp));
}
xml_parser_free($xp);

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
