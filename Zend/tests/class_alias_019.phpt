--TEST--
Trying to redeclare class name in global scope inside namespace
--FILE--
<?php

namespace foo;


class foo {
}

class_alias(__NAMESPACE__ .'\foo', 'foo');
class_alias('\foo', 'foo');

?>
--EXPECTF--
Warning: Cannot declare class foo, because the name is already in use in %s on line %d
