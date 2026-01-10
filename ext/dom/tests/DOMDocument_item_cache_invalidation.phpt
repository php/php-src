--TEST--
DOMDocument node list item cache invalidation
--EXTENSIONS--
dom
--FILE--
<?php

echo "-- Switch document test --\n";

$doc = new DOMDocument();
$doc->loadHTML('<p>hello</p><p>world</p>');

$elements = $doc->getElementsByTagName('p');
$elements->item(0); // Activate item cache
$doc->loadHTML('<p>A</p><p>B</p><p>C</p>');
var_dump($elements);
var_dump($elements->item(0)->textContent); // First lookup
var_dump($elements->item(2)->textContent); // Uses cache
var_dump($elements->item(1)->textContent); // Does not use cache

echo "-- Remove cached item test --\n";

$doc = new DOMDocument();
$doc->loadHTML('<p>hello</p><p>world</p><p>!</p>');

$elements = $doc->getElementsByTagName('p');
$item = $elements->item(0); // Activate item cache
var_dump($item->textContent);
$item->remove();
// Now element 0 means "world", and 1 means "!"
unset($item);
$item = $elements->item(1);
var_dump($item->textContent);

echo "-- Removal of cached item in loop test --\n";

$doc = new DOMDocument;
$doc->loadXML( '<root><e i="1"/><e i="2"/><e i="3"/><e i="4"/><e i="5"/><e i="6"/><e i="7"/><e i="8"/><e i="9"/><e i="10"/><e i="11"/><e i="12"/><e i="13"/><e i="14"/><e i="15"/></root>' );
$root = $doc->documentElement;

$i = 0;
$elements = $root->getElementsByTagName('e');
for ($i = 0; $i < 11; $i++) {
  $node = $elements->item($i);
  print $node->getAttribute('i') . ' ';
  if ($i++ % 2 == 0)
    $root->removeChild( $node );
}
print "\n";

?>
--EXPECTF--
-- Switch document test --
object(DOMNodeList)#2 (1) {
  ["length"]=>
  int(3)
}
string(1) "A"
string(1) "C"
string(1) "B"
-- Remove cached item test --
string(5) "hello"
string(1) "!"
-- Removal of cached item in loop test --
1 4 7 10 13 
Fatal error: Uncaught Error: Call to a member function getAttribute() on null in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
