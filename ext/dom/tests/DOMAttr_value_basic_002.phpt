--TEST--
Write non-string $value property
--CREDITS--
Eric Berg <ehberg@gmail.com>
# TestFest Atlanta 2009-05-14
--EXTENSIONS--
dom
--FILE--
<?php
$attr = new DOMAttr('category');
$attr->value = 1;
print $attr->value;
?>
--EXPECT--
1
