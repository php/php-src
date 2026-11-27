--TEST--
#[\Deprecated]: Parent class implements deprecated interface, child class inherits
--FILE--
<?php

#[\Deprecated("please do not use")]
interface DemoInterface {}

class Base implements DemoInterface {}

class Child extends Base {}

?>
--EXPECTF--
Deprecated: Interface DemoInterface implemented by Base is deprecated, please do not use in %s on line %d
