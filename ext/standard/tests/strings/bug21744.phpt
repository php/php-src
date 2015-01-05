--TEST--
Bug #21744 (strip_tags misses exclamation marks in alt text)
--FILE--
<?php
$test = <<< HERE
<a href="test?test\\!!!test">test</a>
<!-- test -->
HERE;

print strip_tags($test, '');
print strip_tags($test, '<a>');
?>
--EXPECT--
test
<a href="test?test\!!!test">test</a>
