--TEST--
XMLReader: Bug #71805 XML files can generate UTF-8 error even if they are UTF-8
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip"; ?>
--FILE--
<?php

TestXML(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'XMLReaderGood_bug71805.xml');
TestXML(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'XMLReaderBad_bug71805.xml');

function TestXML($file) {
	$XR = new XMLReader;
	$XR->open($file, null, LIBXML_NOBLANKS);

	while (($lastRead = $XR->read()) && ($XR->name !== 'records'));
	while (($lastRead = $XR->read()) && ($XR->name !== 'record'));
	while ($lastRead) {
		$xml = $XR->readOuterXML();
		if ($xml === '') {
			$err = '';
			if ($e = libxml_get_last_error()) { $err = $e->message.' (line: '.$e->line.')'; }
			$XR->close();
			echo $file.' : Problem with file'.($err ? ' — '.$err : '').'.';
			echo "\n";
			return;
		}
		while (($lastRead = $XR->next()) && ($XR->name !== 'record'));
	}
	$XR->close();
	echo $file.' : Good!';
	echo "\n";
	return;
}
?>
===DONE===
--EXPECTF--
%sXMLReaderGood_bug71805.xml : Good!
%sXMLReaderBad_bug71805.xml : Good!
===DONE===
