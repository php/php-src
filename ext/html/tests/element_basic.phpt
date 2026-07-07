--TEST--
Html\Element renders a tag with attributes and children
--EXTENSIONS--
html
--FILE--
<?php
use Html\Element;

$el = new Element('a', ['href' => 'https://php.net', 'class' => 'btn'], ['Click me']);
echo $el->__toString(), "\n";
echo $el, "\n"; // echo casts via __toString

var_dump($el instanceof Html\Htmlable);
var_dump($el instanceof Stringable);
var_dump($el->tag);
var_dump($el->attributes);
var_dump($el->children);
?>
--EXPECT--
<a href="https://php.net" class="btn">Click me</a>
<a href="https://php.net" class="btn">Click me</a>
bool(true)
bool(true)
string(1) "a"
array(2) {
  ["href"]=>
  string(15) "https://php.net"
  ["class"]=>
  string(3) "btn"
}
array(1) {
  [0]=>
  string(8) "Click me"
}
