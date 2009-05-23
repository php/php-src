--TEST--
DOMAttr read $name property.
--CREDITS--
Nic Rosental <nicrosental@gmail.com>
# TestFest Atlanta 2009-05-14
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$attr = new DOMAttr('category', 'books');
print $attr->name;
?>
--EXPECT--
category