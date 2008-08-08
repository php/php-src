--TEST--
Bug #37911 (preg_replace_callback ignores named groups)
--FILE--
<?php

function callback($match)
{
	var_dump($match);
	return $match[1].'/'.strlen($match['name']);
}

var_dump(preg_replace_callback('|(?P<name>blub)|', 'callback', 'bla blub blah'));

var_dump(preg_match('|(?P<name>blub)|', 'bla blub blah', $m));
var_dump($m);

var_dump(preg_replace_callback('|(?P<1>blub)|', 'callback', 'bla blub blah'));

?>
--EXPECTF--
array(3) {
  [0]=>
  string(4) "blub"
  ["name"]=>
  string(4) "blub"
  [1]=>
  string(4) "blub"
}
string(15) "bla blub/4 blah"
int(1)
array(3) {
  [0]=>
  string(4) "blub"
  ["name"]=>
  string(4) "blub"
  [1]=>
  string(4) "blub"
}

Warning: preg_replace_callback(): Numeric named subpatterns are not allowed in %sbug37911.php on line 14
NULL
