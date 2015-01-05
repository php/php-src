--TEST--
ReflectionFunction::getFileName() with function in an included file
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php

include "included4.inc";

$funcInfo = new ReflectionFunction('g');
var_dump($funcInfo->getFileName());

?>
--EXPECTF--
%sincluded4.inc
%d
string(%d) "%sincluded4.inc"