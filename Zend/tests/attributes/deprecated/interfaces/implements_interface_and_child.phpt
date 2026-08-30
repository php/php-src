--TEST--
#[\Deprecated]: Implementing a deprecated interface and a child of it
--FILE--
<?php

#[\Deprecated("please do not use")]
interface BaseInterface {}

interface ChildInterface extends BaseInterface {}

class DemoClass implements BaseInterface, ChildInterface {}

?>
--EXPECTF--
Deprecated: Interface BaseInterface extended by ChildInterface is deprecated, please do not use in %s on line %d

Deprecated: Interface BaseInterface implemented by DemoClass is deprecated, please do not use in %s on line %d
