--TEST--
#[\Deprecated]: Parent class implements deprecated interface, child class inherits and also implements
--FILE--
<?php

#[\Deprecated("please do not use")]
interface DemoInterface {}

class Base implements DemoInterface {}

class Child extends Base implements DemoInterface {}

?>
--EXPECTF--
Deprecated: Interface DemoInterface implemented by Base is deprecated, please do not use in %s on line %d

Deprecated: Interface DemoInterface implemented by Child is deprecated, please do not use in %s on line %d
