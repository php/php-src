--TEST--
PDO OCI Bug #60994 (Reading a multibyte CLOB caps at 8192 characters)
--CREDITS--
Chuck Burgess
ashnazg@php.net
--SKIPIF--
<?php
if (!extension_loaded('mbstring') || !extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require __DIR__.'/../../pdo/tests/pdo_test.inc';
if (!strpos(strtolower(getenv('PDOTEST_DSN')), 'charset=al32utf8')) die('skip expected output valid for AL32UTF8 character set');
PDOTest::skip();
?>
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$dbh = PDOTest::factory();
$dbh->setAttribute(PDO::ATTR_CASE, PDO::CASE_NATURAL);
$dbh->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

@$dbh->exec('DROP TABLE pdo_oci_bug60994');
$dbh->exec('CREATE TABLE pdo_oci_bug60994 (id NUMBER, data CLOB)');

$id = null;
$insert = $dbh->prepare('INSERT INTO pdo_oci_bug60994 (id, data) VALUES (:id, :data)');
$insert->bindParam(':id', $id, \PDO::PARAM_STR);
$select = $dbh->prepare("SELECT data FROM pdo_oci_bug60994 WHERE id = :id");


echo PHP_EOL, 'Test 1:  j', PHP_EOL;
$string1 = 'abc' . str_repeat('j', 8187) . 'xyz'; // 8193 chars total works fine here (even 1 million works fine, subject to memory_limit)
$id = 1;
$insert->bindParam(':data', $string1, \PDO::PARAM_STR, strlen($string1)); // length in bytes
$insert->execute();
$select->bindParam(':id', $id, \PDO::PARAM_STR);
$select->execute();
$row = $select->fetch();
$stream1 = stream_get_contents($row['DATA']);
$start1  = mb_substr($stream1, 0, 10);
$ending1 = mb_substr($stream1, -10);
echo 'size of string1 is ', strlen($string1), ' bytes, ', mb_strlen($string1), ' chars.', PHP_EOL;
echo 'size of stream1 is ', strlen($stream1), ' bytes, ', mb_strlen($stream1), ' chars.', PHP_EOL;
echo 'beg  of stream1 is ', $start1, PHP_EOL;
echo 'end  of stream1 is ', $ending1, PHP_EOL;


echo PHP_EOL, 'Test 2:  £', PHP_EOL;
$string2 = 'abc' . str_repeat('£', 8187) . 'xyz'; // 8193 chars total is when it breaks
$id = 2;
$insert->bindParam(':data', $string2, \PDO::PARAM_STR, strlen($string2)); // length in bytes
$insert->execute();
$select->bindParam(':id', $id, \PDO::PARAM_STR);
$select->execute();
$row = $select->fetch();
$stream2 = stream_get_contents($row['DATA']);
$start2  = mb_substr($stream2, 0, 10);
$ending2 = mb_substr($stream2, -10);
echo 'size of string2 is ', strlen($string2), ' bytes, ', mb_strlen($string2), ' chars.', PHP_EOL;
echo 'size of stream2 is ', strlen($stream2), ' bytes, ', mb_strlen($stream2), ' chars.', PHP_EOL;
echo 'beg  of stream2 is ', $start2, PHP_EOL;
echo 'end  of stream2 is ', $ending2, PHP_EOL;


echo PHP_EOL, 'Test 3:  Җ', PHP_EOL;
$string3 = 'abc' . str_repeat('Җ', 8187) . 'xyz'; // 8193 chars total is when it breaks
$id = 3;
$insert->bindParam(':data', $string3, \PDO::PARAM_STR, strlen($string3)); // length in bytes
$insert->execute();
$select->bindParam(':id', $id, \PDO::PARAM_STR);
$select->execute();
$row = $select->fetch();
$stream3 = stream_get_contents($row['DATA']);
$start3  = mb_substr($stream3, 0, 10);
$ending3 = mb_substr($stream3, -10);
echo 'size of string3 is ', strlen($string3), ' bytes, ', mb_strlen($string3), ' chars.', PHP_EOL;
echo 'size of stream3 is ', strlen($stream3), ' bytes, ', mb_strlen($stream3), ' chars.', PHP_EOL;
echo 'beg  of stream3 is ', $start3, PHP_EOL;
echo 'end  of stream3 is ', $ending3, PHP_EOL;


echo PHP_EOL, 'Test 4:  の', PHP_EOL;
$string4 = 'abc' . str_repeat('の', 8187) . 'xyz'; // 8193 chars total is when it breaks
$id = 4;
$insert->bindParam(':data', $string4, \PDO::PARAM_STR, strlen($string4)); // length in bytes
$insert->execute();
$select->bindParam(':id', $id, \PDO::PARAM_STR);
$select->execute();
$row = $select->fetch();
$stream4 = stream_get_contents($row['DATA']);
$start4  = mb_substr($stream4, 0, 10);
$ending4 = mb_substr($stream4, -10);
echo 'size of string4 is ', strlen($string4), ' bytes, ', mb_strlen($string4), ' chars.', PHP_EOL;
echo 'size of stream4 is ', strlen($stream4), ' bytes, ', mb_strlen($stream4), ' chars.', PHP_EOL;
echo 'beg  of stream4 is ', $start4, PHP_EOL;
echo 'end  of stream4 is ', $ending4, PHP_EOL;
?>
--XFAIL--
Fails due to Bug 60994
--EXPECT--
Test 1:  j
size of string1 is 1000006 bytes, 1000006 chars.
size of stream1 is 1000006 bytes, 1000006 chars.
beg  of stream1 is abcjjjjjjj
end  of stream1 is jjjjjjjxyz

Test 2:  £
size of string2 is 16380 bytes, 8193 chars.
size of stream2 is 16380 bytes, 8193 chars.
beg  of stream2 is abc£££££££
end  of stream2 is £££££££xyz

Test 3:  Җ
size of string3 is 16380 bytes, 8193 chars.
size of stream3 is 16380 bytes, 8193 chars.
beg  of stream3 is abcҖҖҖҖҖҖҖ
end  of stream3 is ҖҖҖҖҖҖҖxyz

Test 4:  の
size of string4 is 24567 bytes, 8193 chars.
size of stream4 is 24567 bytes, 8193 chars.
beg  of stream4 is abcののののののの
end  of stream4 is のののののののxyz
