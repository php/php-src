--TEST--
Bug #44827 (Class error when trying to access :: as constant)
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest Munich 2009
--FILE--
<?php
define('::', true);
try {
    constant('::');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECTF--
Warning: Class constants cannot be defined or redefined in %s on line %d
Couldn't find constant ::
