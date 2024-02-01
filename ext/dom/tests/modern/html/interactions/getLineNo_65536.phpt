--TEST--
getLineNo() returns the line number of the node >= 65536
--EXTENSIONS--
dom
--FILE--
<?php

$html = str_repeat("\n", 65536) . <<<EOF
<!doctype html>
<body>
    <p>hello</p>
</body>
EOF;

$dom = DOM\HTMLDocument::createFromString($html);
var_dump($dom->documentElement->firstChild->nextSibling->firstChild->nextSibling->getLineNo());

?>
--EXPECT--
int(65538)
