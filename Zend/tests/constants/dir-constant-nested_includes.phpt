--TEST--
__DIR__ constant test with nested includes
--FILE--
<?php
echo __DIR__ . "\n";
echo dirname(__FILE__) . "\n";
include 'fixtures/folder1/fixture.php';
include 'fixtures/folder1/subfolder1/fixture.php';
include 'fixtures/folder1/subfolder2/fixture.php';
include 'fixtures/folder1/subfolder3/fixture.php';
include 'fixtures/folder1/subfolder4/fixture.php';
include 'fixtures/folder2/fixture.php';
include 'fixtures/folder2/subfolder1/fixture.php';
include 'fixtures/folder2/subfolder2/fixture.php';
include 'fixtures/folder2/subfolder3/fixture.php';
include 'fixtures/folder2/subfolder4/fixture.php';
include 'fixtures/folder3/fixture.php';
include 'fixtures/folder3/subfolder1/fixture.php';
include 'fixtures/folder3/subfolder2/fixture.php';
include 'fixtures/folder3/subfolder3/fixture.php';
include 'fixtures/folder3/subfolder4/fixture.php';
include 'fixtures/folder4/fixture.php';
include 'fixtures/folder4/subfolder1/fixture.php';
include 'fixtures/folder4/subfolder2/fixture.php';
include 'fixtures/folder4/subfolder3/fixture.php';
include 'fixtures/folder4/subfolder4/fixture.php';
?>
--EXPECTF--
%s/tests/constants
%s/tests/constants
%s/tests/constants/fixtures/folder1
%s/tests/constants/fixtures/folder1
%s/tests/constants/fixtures/folder1/subfolder1
%s/tests/constants/fixtures/folder1/subfolder1
%s/tests/constants/fixtures/folder1/subfolder2
%s/tests/constants/fixtures/folder1/subfolder2
%s/tests/constants/fixtures/folder1/subfolder3
%s/tests/constants/fixtures/folder1/subfolder3
%s/tests/constants/fixtures/folder1/subfolder4
%s/tests/constants/fixtures/folder1/subfolder4
%s/tests/constants/fixtures/folder2
%s/tests/constants/fixtures/folder2
%s/tests/constants/fixtures/folder2/subfolder1
%s/tests/constants/fixtures/folder2/subfolder1
%s/tests/constants/fixtures/folder2/subfolder2
%s/tests/constants/fixtures/folder2/subfolder2
%s/tests/constants/fixtures/folder2/subfolder3
%s/tests/constants/fixtures/folder2/subfolder3
%s/tests/constants/fixtures/folder2/subfolder4
%s/tests/constants/fixtures/folder2/subfolder4
%s/tests/constants/fixtures/folder3
%s/tests/constants/fixtures/folder3
%s/tests/constants/fixtures/folder3/subfolder1
%s/tests/constants/fixtures/folder3/subfolder1
%s/tests/constants/fixtures/folder3/subfolder2
%s/tests/constants/fixtures/folder3/subfolder2
%s/tests/constants/fixtures/folder3/subfolder3
%s/tests/constants/fixtures/folder3/subfolder3
%s/tests/constants/fixtures/folder3/subfolder4
%s/tests/constants/fixtures/folder3/subfolder4
%s/tests/constants/fixtures/folder4
%s/tests/constants/fixtures/folder4
%s/tests/constants/fixtures/folder4/subfolder1
%s/tests/constants/fixtures/folder4/subfolder1
%s/tests/constants/fixtures/folder4/subfolder2
%s/tests/constants/fixtures/folder4/subfolder2
%s/tests/constants/fixtures/folder4/subfolder3
%s/tests/constants/fixtures/folder4/subfolder3
%s/tests/constants/fixtures/folder4/subfolder4
%s/tests/constants/fixtures/folder4/subfolder4
