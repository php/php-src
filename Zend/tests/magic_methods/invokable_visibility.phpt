--TEST--
Non-public __invoke() still auto-implements Invokable (visibility is a separate warning)
--FILE--
<?php

/* PHP warns about non-public __invoke but still allows it.
 * Invokable auto-implementation follows the same behavior as __invoke itself. */
class ProtectedInvoke {
    protected function __invoke(): void {}
}

class PrivateInvoke {
    private function __invoke(): void {}
}

var_dump(new ProtectedInvoke() instanceof Invokable);
var_dump(new PrivateInvoke() instanceof Invokable);

?>
--EXPECTF--
Warning: The magic method ProtectedInvoke::__invoke() must have public visibility in %s on line %d

Warning: The magic method PrivateInvoke::__invoke() must have public visibility in %s on line %d
bool(true)
bool(true)
