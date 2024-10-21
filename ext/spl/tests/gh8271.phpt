--TEST--
GH-8271: NULL pointer dereference in RegexIterator::accept()
--EXTENSIONS--
spl
--FILE--
<?php
function words() {
    yield "f\xC3oo";
    yield "bar";
    yield "baz";
}

$it = new RegexIterator(words(), '/a/u', RegexIterator::REPLACE);
var_dump(iterator_to_array($it));

function wordsForEach() {
    foreach (["f\xC3oo", "bar", "baz"] as $word) {
        yield $word;
    }
}
$it = new RegexIterator(wordsForEach(), '/a/u', RegexIterator::REPLACE);
var_dump(iterator_to_array($it));

?>
--EXPECT--
array(2) {
  [1]=>
  string(2) "br"
  [2]=>
  string(2) "bz"
}
array(2) {
  [1]=>
  string(2) "br"
  [2]=>
  string(2) "bz"
}
