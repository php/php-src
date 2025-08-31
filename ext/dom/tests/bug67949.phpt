--TEST--
Bug #67949: DOMNodeList elements should be accessible through array notation
--EXTENSIONS--
dom
--FILE--
<?php

// Note: non-numeric string accesses fail on NodeLists (as per spec).

$html = <<<HTML
<div>data</div>
<a href="test">hello world</a>
HTML;
$doc = new DOMDocument;
$doc->loadHTML($html);

$nodes = $doc->getElementsByTagName('div');

echo "--- testing has_dimension ---\n";
var_dump(isset($nodes[0]));
var_dump(isset($nodes[1]));
var_dump(isset($nodes[-1]));

echo "--- testing property access ---\n";
var_dump($nodes[0]->textContent);
var_dump($nodes[1]->textContent);

echo "--- testing offset not a long: array ---\n";
$offset = ['test'];
var_dump($offset);
var_dump(isset($nodes[$offset]), $nodes[$offset]->textContent);

echo "--- testing offset not a long: Reference to string ---\n";
$something = 'href';
$offset = &$something;

var_dump($offset);
var_dump(isset($nodes[$offset]), $nodes[$offset]->textContent);

echo "--- testing offset not a long: string ---\n";
$offset = 'test';
var_dump($offset);
var_dump(isset($nodes[$offset]), $nodes[$offset]->textContent);

echo "--- testing read_dimension with null offset ---\n";
try {
    var_dump($nodes[][] = 1);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "--- testing attribute access ---\n";
$anchor = $doc->getElementsByTagName('a')[0];
var_dump($anchor->attributes[0]->name);

echo "==DONE==\n";
?>
--EXPECTF--
--- testing has_dimension ---
bool(true)
bool(false)
bool(false)
--- testing property access ---
string(4) "data"

Warning: Attempt to read property "textContent" on null in %s on line %d
NULL
--- testing offset not a long: array ---
array(1) {
  [0]=>
  string(4) "test"
}

Warning: Attempt to read property "textContent" on null in %s on line %d
bool(false)
NULL
--- testing offset not a long: Reference to string ---
string(4) "href"

Warning: Attempt to read property "textContent" on null in %s on line %d
bool(false)
NULL
--- testing offset not a long: string ---
string(4) "test"

Warning: Attempt to read property "textContent" on null in %s on line %d
bool(false)
NULL
--- testing read_dimension with null offset ---
Cannot access DOMNodeList without offset
--- testing attribute access ---
string(4) "href"
==DONE==
