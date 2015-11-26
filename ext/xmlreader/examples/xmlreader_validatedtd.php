<?php
$indent = 5; /* Number of spaces to indent per level */

$xml = new XMLReader();
$xml->open("dtdexample.xml");
$xml->setParserProperty(XMLREADER::LOADDTD, TRUE);
$xml->setParserProperty(XMLREADER::VALIDATE, TRUE);
while($xml->read()) {
	/* Print node name indenting it based on depth and $indent var */
	print str_repeat(" ", $xml->depth * $indent).$xml->name."\n";
	if ($xml->hasAttributes) {
		$attCount = $xml->attributeCount;
		print str_repeat(" ", $xml->depth * $indent)." Number of Attributes: ".$xml->attributeCount."\n";
	}
}
print "\n\nValid:\n";
var_dump($xml->isValid());
?>