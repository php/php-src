--TEST--
Bug #35673 (formatOutput does not work with saveHTML).
--EXTENSIONS--
dom
--FILE--
<?php
$html = '<html><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>This is the title</title></head></html>';

$htmldoc = new DOMDocument();
$htmldoc->loadHTML($html);
$htmldoc->formatOutput = true;
echo $htmldoc->saveHTML();
?>
--EXPECT--
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "http://www.w3.org/TR/REC-html40/loose.dtd">
<html><head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>This is the title</title>
</head></html>
