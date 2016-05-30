--TEST--
Assert dynamic reference string variables can be imported to function scope with T_USE
--FILE--
<?php
${'+'} = 10;

$test_ref = function () use (&${'+'}) {
  ${'+'} = 20;
};

$test_ref();

echo ${'+'};
?>
--EXPECT--
20
