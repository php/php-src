--TEST--
Bug #38354 (Unwanted reformatting of XML when using AsXML)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$xml = simplexml_load_string(
'<code>
	<a href="javascript:alert(\'1\');"><strong>Item Two</strong></a>
</code>'
);

foreach ($xml->xpath("//*") as $element) {
	var_dump($element->asXML());
}

echo "Done\n";
?>
--EXPECT--
string(101) "<?xml version="1.0"?>
<code>
	<a href="javascript:alert('1');"><strong>Item Two</strong></a>
</code>
"
string(62) "<a href="javascript:alert('1');"><strong>Item Two</strong></a>"
string(25) "<strong>Item Two</strong>"
Done
