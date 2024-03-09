--TEST--
Document::getElementsByTagNameNS
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/create_element_util.inc';

$dom = DOM\HTMLDocument::createFromString(<<<HTML
<!DOCTYPE html>
<html>
<head>
    <title>Test</title>
</head>
<body>
    <p>Hello World</p>
</body>
</html>
HTML);

$body = $dom->getElementsByTagName('body')->item(0);
$body->appendChild(createElementNS($dom, NULL, "p", "content 1"));
$body->appendChild(createElementNS($dom, "", "p", "content 2"));
$body->appendChild(createElementNS($dom, "http://www.w3.org/2000/svg", "svg:svg", "content 3"));

function dump($dom, $uri, $local) {
    $list = $dom->getElementsByTagNameNS($uri, $local);

    $uri_readable = is_null($uri) ? "NULL" : "\"$uri\"";
    echo "--- ($uri_readable, \"$local\"): {$list->length} ---\n";

    var_dump($list->length);
    foreach ($list as $item) {
        var_dump($item->textContent);
    }
}

dump($dom, 'http://www.w3.org/1999/xhtml', 'p');
dump($dom, '*', 'p');
dump($dom, '*', '*');
dump($dom, '', '*');
dump($dom, NULL, '*');
dump($dom, '*', 'svg');
dump($dom, '*', 'svg:svg');

?>
--EXPECT--
--- ("http://www.w3.org/1999/xhtml", "p"): 1 ---
int(1)
string(11) "Hello World"
--- ("*", "p"): 3 ---
int(3)
string(11) "Hello World"
string(9) "content 1"
string(9) "content 2"
--- ("*", "*"): 8 ---
int(8)
string(56) "
    Test


    Hello World

content 1content 2content 3"
string(10) "
    Test
"
string(4) "Test"
string(45) "
    Hello World

content 1content 2content 3"
string(11) "Hello World"
string(9) "content 1"
string(9) "content 2"
string(9) "content 3"
--- ("", "*"): 2 ---
int(2)
string(9) "content 1"
string(9) "content 2"
--- (NULL, "*"): 2 ---
int(2)
string(9) "content 1"
string(9) "content 2"
--- ("*", "svg"): 1 ---
int(1)
string(9) "content 3"
--- ("*", "svg:svg"): 0 ---
int(0)
