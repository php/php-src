--TEST--
Bug #80332 (Completely broken array access functionality with DOMNamedNodeMap) - DOMNodeList variation
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;
$doc->loadXML('<?xml version="1.0"?><span><strong id="1"/><strong id="2"/></span>');

$list = $doc->getElementsByTagName('strong');

function test($list, $key) {
    $key_formatted = match ($key) {
        false => 'false',
        true => 'true',
        null => 'null',
        default => is_string($key) ? "'$key'" : $key,
    };
    echo "list[$key_formatted] id attribute: ", $list[$key] ? $list[$key]->getAttribute('id') : '/', "\n";
}

test($list, 0);
test($list, false);
test($list, true);
test($list, null);
test($list, '0');
test($list, '0.5');
test($list, '1');
// These two should fail because there's no named lookup possible here
test($list, 'attr2');
test($list, 'hi');
// This one should fail because it's out of bounds
test($list, '2147483647');

?>
--EXPECT--
list[0] id attribute: 1
list[false] id attribute: 1
list[true] id attribute: 2
list[null] id attribute: 1
list['0'] id attribute: 1
list['0.5'] id attribute: 1
list['1'] id attribute: 2
list['attr2'] id attribute: /
list['hi'] id attribute: /
list['2147483647'] id attribute: /
