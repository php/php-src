<?php
$xmlstring = '<books>
 <book num="1">
  <title>The Grapes of Wrath</title>
  <author>John Steinbeck</author>
 </book>
 <book num="2">
  <title>The Pearl</title>
  <author>John Steinbeck</author>
 </book>
</books>';

$reader = new XMLReader();
$reader->XML($xmlstring);
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
