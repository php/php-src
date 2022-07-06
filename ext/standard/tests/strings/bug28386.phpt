--TEST--
Bug #28386 (wordwrap() wraps text 1 character too soon)
--FILE--
<?php
$text = "Some text";
$string = "$text $text $text $text";
echo wordwrap($string, 9);
?>
--EXPECT--
Some text
Some text
Some text
Some text
