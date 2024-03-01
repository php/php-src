--TEST--
DOM\HTMLDocument: overrideEncoding with incompatible charset
--EXTENSIONS--
iconv
dom
--FILE--
<?php
$doc = DOM\HTMLDocument::createFromString(
	iconv(
		'ISO-8859-1',
		'UTF-8',
		<<<'DOC'
			<!DOCTYPE html>
			<html>
				<head>
					<meta charset="iso-8859-1">
					<title>äöü</title>
				</head>
				<body>
					äöü
				</body>
			</html>
			DOC,
	),
	overrideEncoding: 'utf-8'
);

var_dump(iconv('UTF-8', 'ISO-8859-1', $doc->getElementsByTagName('title')->item(0)->textContent));
var_dump(iconv('UTF-8', 'ISO-8859-1', $doc->getElementsByTagName('body')->item(0)->textContent));
?>
--EXPECT--
string(3) "äöü"
string(9) "
		äöü
	
"
