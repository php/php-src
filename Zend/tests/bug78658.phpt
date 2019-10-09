--TEST--
Bug #78658: Memory corruption using Closure::bindTo()
--FILE--
<?php

$c = function(){};

$scope = "AAAA";
$scope = "{$scope}BBBB";
$c->bindTo(new stdClass, $scope);

?>
--EXPECTF--
Warning: Class 'AAAABBBB' not found in %s on line %d
