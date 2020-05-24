--TEST--
Attributes: Prevent PhpAttribute on non classes
--FILE--
<?php

<<PhpAttribute>>
function foo() {}
--EXPECTF--
Fatal error: Only classes can be marked with <<PhpAttribute>> in %s
