--TEST--
Tests for LOBs with multibyte strings, reading them out in chunks
(Doc Bug #70700)
--CREDITS--
Chuck Burgess
ashnazg@php.net
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring is not enabled');
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--ENV--
NLS_LANG=.AL32UTF8
--FILE--
<?php
require(__DIR__.'/connect.inc');


$stmt = oci_parse($c, 'DROP TABLE oci8_bug70700');
@oci_execute($stmt);
oci_free_statement($stmt);
$stmt = oci_parse($c, 'CREATE TABLE oci8_bug70700 (id NUMBER, data CLOB)');
oci_execute($stmt);
oci_free_statement($stmt);


$id = null;
$insert = oci_parse($c, 'INSERT INTO oci8_bug70700 (id, data) VALUES (:id, :data)');
oci_bind_by_name($insert, ':id', $id);
$select = oci_parse($c, "SELECT data FROM oci8_bug70700 WHERE id = :id");
oci_bind_by_name($select, ':id', $id);


echo PHP_EOL, 'Test 1: j', PHP_EOL;
$string1 = 'abc' . str_repeat('j', 1000000) . 'xyz';
$id = 1;
$desc = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($insert, ':data', $desc, -1, OCI_B_CLOB);
$desc->writeTemporary($string1, OCI_TEMP_CLOB);
oci_execute($insert);
$desc->save($string1);
oci_commit($c);
$desc->close();
oci_bind_by_name($select, ':id', $id);
oci_execute($select);
$row = oci_fetch_array($select, OCI_ASSOC);
$lob = $row['DATA'];
$fh = fopen('php://temp', 'rw');
while (! $lob->eof()) {
    $data = $lob->read(8192);             // read($characters), not read($bytes)
    fwrite($fh, $data, strlen($data));    // fwrite(a, b, $bytes)
}
$lob->free();
rewind($fh);
$stream1a = stream_get_contents($fh);
fclose($fh);
$start1a = mb_substr($stream1a, 0, 10);
$ending1a = mb_substr($stream1a, -10);
echo 'size of string1 is ', strlen($string1), ' bytes, ', mb_strlen($string1), ' chars.', PHP_EOL;
echo 'size of stream1a is ', strlen($stream1a), ' bytes, ', mb_strlen($stream1a), ' chars.', PHP_EOL;
echo 'beg of stream1a is ', $start1a, PHP_EOL;
echo 'end of stream1a is ', $ending1a, PHP_EOL;


echo PHP_EOL, 'Test 2: £', PHP_EOL;
$string2 = 'abc' . str_repeat('£', 4094) . 'xyz';
$id = 2;
$desc = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($insert, ':data', $desc, -1, OCI_B_CLOB);
$desc->writeTemporary($string2, OCI_TEMP_CLOB);
oci_execute($insert);
$desc->save($string2);
oci_commit($c);
$desc->close();
oci_bind_by_name($select, ':id', $id);
oci_execute($select);
$row = oci_fetch_array($select, OCI_ASSOC);
$lob = $row['DATA'];
$fh = fopen('php://temp', 'rw');
while (! $lob->eof()) {
    $data = $lob->read(8192);             // read($characters), not read($bytes)
    fwrite($fh, $data, strlen($data));    // fwrite(a, b, $bytes)
}
$lob->free();
rewind($fh);
$stream2a = stream_get_contents($fh);
fclose($fh);
$start2a = mb_substr($stream2a, 0, 10);
$ending2a = mb_substr($stream2a, -10);
echo 'size of string2 is ', strlen($string2), ' bytes, ', mb_strlen($string2), ' chars.', PHP_EOL;
echo 'size of stream2a is ', strlen($stream2a), ' bytes, ', mb_strlen($stream2a), ' chars.', PHP_EOL;
echo 'beg of stream2a is ', $start2a, PHP_EOL;
echo 'end of stream2a is ', $ending2a, PHP_EOL;


echo PHP_EOL, 'Test 3: Җ', PHP_EOL;
$string3 = 'abc' . str_repeat('Җ', 4094) . 'xyz';
$id = 3;
$desc = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($insert, ':data', $desc, -1, OCI_B_CLOB);
$desc->writeTemporary($string3, OCI_TEMP_CLOB);
oci_execute($insert);
$desc->save($string3);
oci_commit($c);
$desc->close();
oci_bind_by_name($select, ':id', $id);
oci_execute($select);
$row = oci_fetch_array($select, OCI_ASSOC);
$lob = $row['DATA'];
$fh = fopen('php://temp', 'rw');
while (! $lob->eof()) {
    $data = $lob->read(8192);             // read($characters), not read($bytes)
    fwrite($fh, $data, strlen($data));    // fwrite(a, b, $bytes)
}
$lob->free();
rewind($fh);
$stream3a = stream_get_contents($fh);
fclose($fh);
$start3a = mb_substr($stream3a, 0, 10);
$ending3a = mb_substr($stream3a, -10);
echo 'size of string3 is ', strlen($string3), ' bytes, ', mb_strlen($string3), ' chars.', PHP_EOL;
echo 'size of stream3a is ', strlen($stream3a), ' bytes, ', mb_strlen($stream3a), ' chars.', PHP_EOL;
echo 'beg of stream3a is ', $start3a, PHP_EOL;
echo 'end of stream3a is ', $ending3a, PHP_EOL;


echo PHP_EOL, 'Test 4: の', PHP_EOL;
$string4 = 'abc' . str_repeat('の', 2729) . 'xyz';
$id = 4;
$desc = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($insert, ':data', $desc, -1, OCI_B_CLOB);
$desc->writeTemporary($string4, OCI_TEMP_CLOB);
oci_execute($insert);
$desc->save($string4);
oci_commit($c);
$desc->close();
oci_bind_by_name($select, ':id', $id);
oci_execute($select);
$row = oci_fetch_array($select, OCI_ASSOC);
$lob = $row['DATA'];
$fh = fopen('php://temp', 'rw');
while (! $lob->eof()) {
    $data = $lob->read(8192);             // read($characters), not read($bytes)
    fwrite($fh, $data, strlen($data));    // fwrite(a, b, $bytes)
}
$lob->free();
rewind($fh);
$stream4a = stream_get_contents($fh);
fclose($fh);
$start4a = mb_substr($stream4a, 0, 10);
$ending4a = mb_substr($stream4a, -10);
echo 'size of string4 is ', strlen($string4), ' bytes, ', mb_strlen($string4), ' chars.', PHP_EOL;
echo 'size of stream4a is ', strlen($stream4a), ' bytes, ', mb_strlen($stream4a), ' chars.', PHP_EOL;
echo 'beg of stream4a is ', $start4a, PHP_EOL;
echo 'end of stream4a is ', $ending4a, PHP_EOL;
--EXPECT--
Test 1: j
size of string1 is 1000006 bytes, 1000006 chars.
size of stream1a is 1000006 bytes, 1000006 chars.
beg of stream1a is abcjjjjjjj
end of stream1a is jjjjjjjxyz

Test 2: £
size of string2 is 8194 bytes, 4100 chars.
size of stream2a is 8194 bytes, 4100 chars.
beg of stream2a is abc£££££££
end of stream2a is £££££££xyz

Test 3: Җ
size of string3 is 8194 bytes, 4100 chars.
size of stream3a is 8194 bytes, 4100 chars.
beg of stream3a is abcҖҖҖҖҖҖҖ
end of stream3a is ҖҖҖҖҖҖҖxyz

Test 4: の
size of string4 is 8193 bytes, 2735 chars.
size of stream4a is 8193 bytes, 2735 chars.
beg of stream4a is abcののののののの
end of stream4a is のののののののxyz
