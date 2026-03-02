--TEST--
Bug #81101 - Invalid single character repetition issues in JIT
--FILE--
<?php

$matches = [];
$test = ' App\Domain\Repository\MetaData\SomethingRepositoryInterface';

preg_match('/\\\\([^\\\\]+)\s*$/', $test, $matches);
var_dump($matches);

$test2 = ' App\Domain\Exception\NotFoundException';

preg_match('/\\\\([^\\\\]+)\s*$/', $test2, $matches);
var_dump($matches);

?>
--EXPECT--
array(2) {
  [0]=>
  string(29) "\SomethingRepositoryInterface"
  [1]=>
  string(28) "SomethingRepositoryInterface"
}
array(2) {
  [0]=>
  string(18) "\NotFoundException"
  [1]=>
  string(17) "NotFoundException"
}
