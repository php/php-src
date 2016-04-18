--TEST--
Callable should be accepted if it expects less arguments than what was declared in a typehint
--FILE--
<?php

$a = function (callable(callable($a, $b)) $a) { $a(function ($a) {  }); };
$a(function (callable($a, $b) $a) { $a(1, 2); });

$a = function (callable($a) $cb) { $cb(12); };
$a(function () {});

?>
--EXPECT--
