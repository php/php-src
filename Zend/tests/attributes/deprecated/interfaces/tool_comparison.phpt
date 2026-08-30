--TEST--
#[\Deprecated]: Interface deprecation RFC example
--FILE--
<?php

#[\Deprecated]
interface BaseInterface {}

interface ChildInterface extends BaseInterface {}

class ImplementsBase implements BaseInterface {}

class ImplementsBoth implements BaseInterface, ChildInterface {}

class ImplementsChild implements ChildInterface {}

class ExtendsBaseImplementor extends ImplementsBase {}

?>
--EXPECTF--
Deprecated: Interface BaseInterface extended by ChildInterface is deprecated in %s on line %d

Deprecated: Interface BaseInterface implemented by ImplementsBase is deprecated in %s on line %d

Deprecated: Interface BaseInterface implemented by ImplementsBoth is deprecated in %s on line %d
