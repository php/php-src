--TEST--
Read empty $value. 
--CREDIT--
Jason Bouffard <jbouffard1@yahoo.com>
# TestFest Atlanta 2009-05-14
--FILE--
<?php
$attr = new DOMAttr('category');
print $attr->value."\n";
?>
--EXPECTF--
