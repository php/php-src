--TEST--
ReflectionClass::getExtensionName() method - basic test for getExtensionName() method
--EXTENSIONS--
dom
--CREDITS--
Rein Velt <rein@velt.org>
#testFest Roosendaal 2008-05-10
--FILE--
<?php
    $rc=new reflectionClass('domDocument');
    var_dump( $rc->getExtensionName()) ;
?>
--EXPECT--
string(3) "dom"
