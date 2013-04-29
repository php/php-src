--TEST--
Write non-string $value property
--CREDITS--
Eric Berg <ehberg@gmail.com>
# TestFest Atlanta 2009-05-14
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$attr = new DOMAttr('category');
$attr->value = 1;
print $attr->value;
?>
--EXPECTF--
1
