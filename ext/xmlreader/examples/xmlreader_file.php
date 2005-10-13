<?php
$reader = new XMLReader();
$reader->open('xmlreader.xml');
while ($reader->read()) {
	if ($reader->nodeType != XMLREADER::END_ELEMENT) {
		print "Node Name: ".$reader->name."\n";
		print "Node Value: ".$reader->value."\n";
		print "Node Depth: ".$reader->depth."\n";
		if ($reader->nodeType==XMLREADER::ELEMENT && $reader->hasAttributes) {
			$attr = $reader->moveToFirstAttribute();
			while ($attr) {
				print "   Attribute Name: ".$reader->name."\n";
				print "   Attribute Value: ".$reader->value."\n";
				$attr = $reader->moveToNextAttribute();
			}
		}
		print "\n";
	}
}
?>
