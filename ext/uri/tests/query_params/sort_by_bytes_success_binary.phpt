--TEST--
Test Uri\QueryParams::sortByBytes() - success - binary names and values
--FILE--
<?php

$params1 = (new Uri\QueryParams())
    ->append("\xFF", "first")
    ->append("\u{FFFD}", "literal")
    ->append("\xE2\x82", "incomplete")
    ->append("\x80", "continuation")
    ->append("\xE2a", "bad-tail")
    ->append("\xED\xA0\x80", "surrogate")
    ->append("\xF4\x90\x80\x80", "out-of-range")
    ->append("\0z", "nul-z")
    ->append("\0a", "nul-a")
    ->append("", "empty")
    ->append("\u{FEFF}", "bom")
    ->append("a", "\xFF")
    ->append("\u{10000}", "supplementary")
    ->append("\xFF", null);
$params2 = $params1->sortByBytes();

var_dump($params2->toRfc3986String());
var_dump($params2->getAll("\xFF"));
var_dump($params2->getAll("\u{FFFD}"));
var_dump($params2->getAll("\xE2\x82"));
var_dump(iterator_to_array($params2) === $params2->toList());

?>
--EXPECT--
string(202) "=empty&%00a=nul-a&%00z=nul-z&a=%FF&%80=continuation&%E2a=bad-tail&%E2%82=incomplete&%ED%A0%80=surrogate&%EF%BB%BF=bom&%EF%BF%BD=literal&%F0%90%80%80=supplementary&%F4%90%80%80=out-of-range&%FF=first&%FF"
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  NULL
}
array(1) {
  [0]=>
  string(7) "literal"
}
array(1) {
  [0]=>
  string(10) "incomplete"
}
bool(true)
