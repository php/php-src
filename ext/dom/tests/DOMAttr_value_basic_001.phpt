--TEST--
Read empty $value.
--CREDITS--
Jason Bouffard <jbouffard1@yahoo.com>
# TestFest Atlanta 2009-05-14
--EXTENSIONS--
dom
--FILE--
<?php
$attr = new DOMAttr('category');
print $attr->value."\n";
?>
===DONE===
--EXPECT--
===DONE===
