--TEST--
__DIR__ constant test with includes
--FILE--
<?php
echo __DIR__ . "\n";
echo dirname(__FILE__) . "\n";
include 'fixtures/folder1/fixture.php';
include 'fixtures/folder2/fixture.php';
include 'fixtures/folder3/fixture.php';
include 'fixtures/folder4/fixture.php';
?>
--EXPECTF--
%stests%sconstants
%stests%sconstants
%stests%sconstants%sfixtures%sfolder1
%stests%sconstants%sfixtures%sfolder1
%stests%sconstants%sfixtures%sfolder2
%stests%sconstants%sfixtures%sfolder2
%stests%sconstants%sfixtures%sfolder3
%stests%sconstants%sfixtures%sfolder3
%stests%sconstants%sfixtures%sfolder4
%stests%sconstants%sfixtures%sfolder4
