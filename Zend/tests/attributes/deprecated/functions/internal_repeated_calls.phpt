--TEST--
#[\Deprecated]: the message of an internal function is the same on every call
--FILE--
<?php

$rp = new ReflectionProperty(new class { public $p = 1; }, 'p');

$rp->setAccessible(true);
$rp->setAccessible(true);
(Closure::fromCallable([$rp, 'setAccessible']))(true);
(new ReflectionMethod($rp, 'setAccessible'))->invoke($rp, true);

set_error_handler(function (int $type, string $message) {
    echo "handler: $message\n";
    return true;
});
$rp->setAccessible(true);
restore_error_handler();
$rp->setAccessible(true);

?>
--EXPECTF--
Deprecated: Method ReflectionProperty::setAccessible() is deprecated since 8.5, as it has no effect since PHP 8.1 in %s on line %d

Deprecated: Method ReflectionProperty::setAccessible() is deprecated since 8.5, as it has no effect since PHP 8.1 in %s on line %d

Deprecated: Method ReflectionProperty::setAccessible() is deprecated since 8.5, as it has no effect since PHP 8.1 in %s on line %d

Deprecated: Method ReflectionProperty::setAccessible() is deprecated since 8.5, as it has no effect since PHP 8.1 in %s on line %d
handler: Method ReflectionProperty::setAccessible() is deprecated since 8.5, as it has no effect since PHP 8.1

Deprecated: Method ReflectionProperty::setAccessible() is deprecated since 8.5, as it has no effect since PHP 8.1 in %s on line %d
