--TEST--
#[\Deprecated]: Implementing a child of a deprecated interface
--FILE--
<?php

#[\Deprecated("please do not use")]
interface BaseInterface {}

interface ChildInterface extends BaseInterface {}

class DemoClass implements ChildInterface {}

?>
--EXPECTF--
Deprecated: Interface BaseInterface extended by ChildInterface is deprecated, please do not use in %s on line %d
