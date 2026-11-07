--TEST--
Str\Matcher results retain keywords after inputs and matcher are destroyed
--EXTENSIONS--
strmatch
--FILE--
<?php
$keyword = str_repeat('x', 64);
$m = new Str\Matcher([&$keyword, 'x']);
$results = $m->matchAll($keyword . $keyword);
$keywords = $m->getKeywords();
$matched = $m->getMatchedKeywords($keyword);
$keyword = 'changed';
unset($m);
var_dump(count($results), $results[1]->length, strlen($results[1]->keyword));
var_dump(strlen($keywords[0]), strlen($matched[1]));
$keywords[0][0] = 'Y';
var_dump($results[1]->keyword[0], $matched[1][0]);
?>
--EXPECT--
int(193)
int(64)
int(64)
int(64)
int(64)
string(1) "x"
string(1) "x"
