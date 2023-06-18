--TEST--
Bug #67949: DOMNodeList elements should be accessible through array notation
--EXTENSIONS--
dom
--FILE--
<?php

$html = <<<HTML
<div>data</div>
<a href="test">hello world</a>
HTML;
$doc = new DOMDocument;
$doc->loadHTML($html);

$nodes = $doc->getElementsByTagName('div');

echo "testing has_dimension\n";
var_dump(isset($nodes[0]));
var_dump(isset($nodes[1]));
var_dump(isset($nodes[-1]));

echo "testing property access\n";
var_dump($nodes[0]->textContent);
var_dump($nodes[1]->textContent);

echo "testing offset not a long\n";
$offset = ['test'];
var_dump($offset);
var_dump(isset($nodes[$offset]), $nodes[$offset]->textContent);
var_dump($offset);

$something = 'test';
$offset = &$something;

var_dump($offset);
var_dump(isset($nodes[$offset]), $nodes[$offset]->textContent);
var_dump($offset);

$offset = 'test';
var_dump($offset);
var_dump(isset($nodes[$offset]), $nodes[$offset]->textContent);
var_dump($offset);

echo "testing read_dimension with null offset\n";
try {
    var_dump($nodes[][] = 1);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "testing attribute access\n";
$anchor = $doc->getElementsByTagName('a')[0];
var_dump($anchor->attributes[0]->name);

echo "==DONE==\n";
?>
--EXPECTF--
testing has_dimension
bool(true)
bool(false)
bool(false)
testing property access
string(4) "data"

Warning: Attempt to read property "textContent" on null in %s on line %d
NULL
testing offset not a long
array(1) {
  [0]=>
  string(4) "test"
}

Warning: Attempt to read property "textContent" on null in %s on line %d
bool(false)
NULL
array(1) {
  [0]=>
  string(4) "test"
}
string(4) "test"
bool(true)
string(4) "data"
string(4) "test"
string(4) "test"
bool(true)
string(4) "data"
string(4) "test"
testing read_dimension with null offset
Cannot access DOMNodeList without offset
testing attribute access
string(4) "href"
==DONE==
