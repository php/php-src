--TEST--
#[\Deprecated]: Interfaces extending deprecated interfaces trigger warnings
--FILE--
<?php

#[\Deprecated("please do not use")]
interface DemoInterface1 {}

#[\Deprecated("will be removed in 3.0", since: "2.7")]
interface DemoInterface2 {}

#[\Deprecated(message: "going away")]
interface DemoInterface3 {}

#[\Deprecated(since: "3.5")]
interface DemoInterface4 {}

interface ExtendsDemo1 extends DemoInterface1 {}
interface ExtendsDemo2 extends DemoInterface2 {}
interface ExtendsDemo3 extends DemoInterface3 {}
interface ExtendsDemo4 extends DemoInterface4 {}

?>
--EXPECTF--
Deprecated: Interface DemoInterface1 extended by ExtendsDemo1 is deprecated, please do not use in %s on line %d

Deprecated: Interface DemoInterface2 extended by ExtendsDemo2 is deprecated since 2.7, will be removed in 3.0 in %s on line %d

Deprecated: Interface DemoInterface3 extended by ExtendsDemo3 is deprecated, going away in %s on line %d

Deprecated: Interface DemoInterface4 extended by ExtendsDemo4 is deprecated since 3.5 in %s on line %d
