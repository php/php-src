--TEST--
XMLReader: libxml2 XML Reader, attributes test
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip"; ?>
--FILE--
<?php
/* $Id$ */
$filename = dirname(__FILE__) . '/_003.xml';

$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books><book num="1" idx="2">book1</book></books>';
file_put_contents($filename, $xmlstring);

$reader = new XMLReader();
if (!$reader->open($filename)) {
	exit();
}

// Only go through
while ($reader->read()) {
	if ($reader->nodeType != XMLREADER::END_ELEMENT) {
		if ($reader->nodeType == XMLREADER::ELEMENT && $reader->hasAttributes) {
			$attr = $reader->moveToFirstAttribute();
			echo $reader->name . ": ";
			echo $reader->value . "\n";

			if ($reader->getAttribute($reader->name) == $reader->value) {
				echo "1st attr (num) failed\n";
			}


			$attr = $reader->moveToNextAttribute();
			echo $reader->name . ": ";
			echo $reader->value . "\n";

			if ($reader->getAttribute($reader->name) == $reader->value) {
				echo "2nd attr (idx) failed\n";
			}

			// Named attribute
			$attr = $reader->moveToAttribute('num');
			echo $reader->name . ": ";
			echo $reader->value . "\n";

			if ($reader->getAttribute('num') == $reader->value) {
				echo "attr num failed\n";
			}

			$attr = $reader->moveToAttribute('idx');
			echo $reader->name . ": ";
			echo $reader->value . "\n";

			if ($reader->getAttribute('idx') == $reader->value) {
				echo "attr idx failed\n";
			}

			// Numeric positions of attributes
			$attr = $reader->moveToAttributeNo(0);
			echo $reader->name . ": ";
			echo $reader->value . "\n";

			if ($reader->getAttributeNo(0) == $reader->value) {
				echo "attr 0 failed\n";
			}

			$attr = $reader->moveToAttributeNo(1);
			echo $reader->name . ": ";
			echo $reader->value . "\n";

		}
	}
}
$reader->close();
unlink($filename);
?>
===DONE===
--EXPECT--
num: 1
idx: 2
num: 1
idx: 2
num: 1
idx: 2
===DONE===
