--TEST--
Delayed early binding throws class redeclaration error
--EXTENSIONS--
opcache
--FILE--
<?php
class Foo {}
include __DIR__ . '/delayed_early_binding_redeclaration-1.inc';
include __DIR__ . '/delayed_early_binding_redeclaration-2.inc';
var_dump(class_exists(Bar::class));
?>
--EXPECTF--
Fatal error: Cannot redeclare class Bar (previously declared in %sdelayed_early_binding_redeclaration-1.inc:2) in %sdelayed_early_binding_redeclaration-2.inc on line %d
