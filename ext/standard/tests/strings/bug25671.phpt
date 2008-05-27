--TEST--
Bug #25671 (subarrays not copied correctly)
--FILE--
<?php
  $arr = array(
	"This is string one.",
	"This is string two.",
	array(
		"This is another string.",
		"This is a last string."),
	"This is a last string.");

  echo serialize(str_replace("string", "strung", $arr)) . "\n";
  echo serialize(str_replace("string", "strung", $arr)) . "\n";
  echo serialize(str_replace(" ", "", $arr)) . "\n";
  echo serialize(str_replace(" ", "", $arr)) . "\n";
?>
--EXPECT--
a:4:{i:0;U:19:"This is strung one.";i:1;U:19:"This is strung two.";i:2;a:2:{i:0;U:23:"This is another string.";i:1;U:22:"This is a last string.";}i:3;U:22:"This is a last strung.";}
a:4:{i:0;U:19:"This is strung one.";i:1;U:19:"This is strung two.";i:2;a:2:{i:0;U:23:"This is another string.";i:1;U:22:"This is a last string.";}i:3;U:22:"This is a last strung.";}
a:4:{i:0;U:16:"Thisisstringone.";i:1;U:16:"Thisisstringtwo.";i:2;a:2:{i:0;U:23:"This is another string.";i:1;U:22:"This is a last string.";}i:3;U:18:"Thisisalaststring.";}
a:4:{i:0;U:16:"Thisisstringone.";i:1;U:16:"Thisisstringtwo.";i:2;a:2:{i:0;U:23:"This is another string.";i:1;U:22:"This is a last string.";}i:3;U:18:"Thisisalaststring.";}
