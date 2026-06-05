--TEST--
DOMDocument::saveHTML() vs DOMDocument::saveXML()
--EXTENSIONS--
dom
--FILE--
<?php
$d = new DOMDocument();
$str = <<<EOD
<html>
<head>
</head>
<body>
<p>Hi.<br/>there</p>
</body>
</html>
EOD;
$d->loadHTML($str);
$e = $d->getElementsByTagName("p");
$e = $e->item(0);
echo $d->saveXML($e),"\n";
echo $d->saveHTML($e),"\n";
?>
--EXPECT--
<p>Hi.<br/>there</p>
<p>Hi.<br>there</p>
