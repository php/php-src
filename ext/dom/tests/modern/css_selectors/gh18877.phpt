--TEST--
GH-18877 (\Dom\HTMLDocument querySelectorAll selecting only the first when using ~ and :has)
--EXTENSIONS--
dom
--FILE--
<?php

$text = <<<TEXT
<html>
<head>
</head>
<body>
    <div><div></div></div>
    <span>1</span>
    <span>2</span>
    <span>3</span>
</body>
</html>
TEXT;

$dom = \Dom\HTMLDocument::createFromString($text, options: LIBXML_NOERROR);
foreach ($dom->querySelectorAll('div:has(div) ~ *') as $node) {
    var_dump($node->textContent);
}

?>
--EXPECT--
string(1) "1"
string(1) "2"
string(1) "3"
