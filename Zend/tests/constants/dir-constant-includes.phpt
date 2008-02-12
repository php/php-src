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
%s/tests/constants
%s/tests/constants
%s/tests/constants/fixtures/folder1
%s/tests/constants/fixtures/folder1
%s/tests/constants/fixtures/folder2
%s/tests/constants/fixtures/folder2
%s/tests/constants/fixtures/folder3
%s/tests/constants/fixtures/folder3
%s/tests/constants/fixtures/folder4
%s/tests/constants/fixtures/folder4
