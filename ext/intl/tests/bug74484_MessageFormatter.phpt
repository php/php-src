--TEST--
Bug #74484 MessageFormatter::formatMessage memory corruption with 11+ named placeholder
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
?>
--FILE--
<?php
$text = "{a} {b} {c} {d} {e} {f} {g} {h} {i} {j} {k} {l}";

$vars = array(
  'a' => 1,
  'b' => 2,
  'c' => 3,
  'd' => 4,
  'e' => 5,
  'f' => 6,
  'g' => 7,
  'h' => 8,
  'i' => 9,
  'j' => 10,
  'k' => 11,
  'l' => 12
);

var_dump(MessageFormatter::formatMessage('en_US', $text, $vars));

?>
==DONE==
--EXPECT--
string(26) "1 2 3 4 5 6 7 8 9 10 11 12"
==DONE==
