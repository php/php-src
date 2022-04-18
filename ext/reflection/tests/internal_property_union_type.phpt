--TEST--
Union type on internal property
--EXTENSIONS--
zend_test
--FILE--
<?php
$rp = new ReflectionProperty(_ZendTestClass::class, 'classUnionProp');
$rt = $rp->getType();
echo $rt, "\n";
?>
--EXPECT--
stdClass|Iterator|null
