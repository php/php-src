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
%stests%sconstants
%stests%sconstants
%stests%sconstants%sfixtures%sfolder1
%stests%sconstants%sfixtures%sfolder1
%stests%sconstants%sfixtures%sfolder1%ssubfolder1
%stests%sconstants%sfixtures%sfolder1%ssubfolder1
%stests%sconstants%sfixtures%sfolder1%ssubfolder2
%stests%sconstants%sfixtures%sfolder1%ssubfolder2
%stests%sconstants%sfixtures%sfolder1%ssubfolder3
%stests%sconstants%sfixtures%sfolder1%ssubfolder3
%stests%sconstants%sfixtures%sfolder1%ssubfolder4
%stests%sconstants%sfixtures%sfolder1%ssubfolder4
%stests%sconstants%sfixtures%sfolder2
%stests%sconstants%sfixtures%sfolder2
%stests%sconstants%sfixtures%sfolder2%ssubfolder1
%stests%sconstants%sfixtures%sfolder2%ssubfolder1
%stests%sconstants%sfixtures%sfolder2%ssubfolder2
%stests%sconstants%sfixtures%sfolder2%ssubfolder2
%stests%sconstants%sfixtures%sfolder2%ssubfolder3
%stests%sconstants%sfixtures%sfolder2%ssubfolder3
%stests%sconstants%sfixtures%sfolder2%ssubfolder4
%stests%sconstants%sfixtures%sfolder2%ssubfolder4
%stests%sconstants%sfixtures%sfolder3
%stests%sconstants%sfixtures%sfolder3
%stests%sconstants%sfixtures%sfolder3%ssubfolder1
%stests%sconstants%sfixtures%sfolder3%ssubfolder1
%stests%sconstants%sfixtures%sfolder3%ssubfolder2
%stests%sconstants%sfixtures%sfolder3%ssubfolder2
%stests%sconstants%sfixtures%sfolder3%ssubfolder3
%stests%sconstants%sfixtures%sfolder3%ssubfolder3
%stests%sconstants%sfixtures%sfolder3%ssubfolder4
%stests%sconstants%sfixtures%sfolder3%ssubfolder4
%stests%sconstants%sfixtures%sfolder4
%stests%sconstants%sfixtures%sfolder4
%stests%sconstants%sfixtures%sfolder4%ssubfolder1
%stests%sconstants%sfixtures%sfolder4%ssubfolder1
%stests%sconstants%sfixtures%sfolder4%ssubfolder2
%stests%sconstants%sfixtures%sfolder4%ssubfolder2
%stests%sconstants%sfixtures%sfolder4%ssubfolder3
%stests%sconstants%sfixtures%sfolder4%ssubfolder3
%stests%sconstants%sfixtures%sfolder4%ssubfolder4
%stests%sconstants%sfixtures%sfolder4%ssubfolder4
