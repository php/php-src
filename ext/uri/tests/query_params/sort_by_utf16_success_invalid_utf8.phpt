--TEST--
Test Uri\QueryParams::sortByUtf16() - malformed UTF-8 compares as U+FFFD and original bytes are preserved
--FILE--
<?php

$params1 = (new Uri\QueryParams())
    ->append("\xE2z", "tail")
    ->append("\xFF", "first")
    ->append("\u{FFFD}", "literal")
    ->append("\xE2\x82", "incomplete")
    ->append("\u{10000}", "pair")
    ->append("\xFF", "last")
    ->append("\u{FFFD}a", "suffix");
$params2 = $params1->sortByUtf16();

var_dump($params2->toRfc3986String());
var_dump($params2->getAll("\xFF"));
var_dump($params2->getFirst("\u{FFFD}"));

?>
--EXPECT--
string(100) "%F0%90%80%80=pair&%FF=first&%EF%BF%BD=literal&%E2%82=incomplete&%FF=last&%EF%BF%BDa=suffix&%E2z=tail"
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(4) "last"
}
string(7) "literal"
