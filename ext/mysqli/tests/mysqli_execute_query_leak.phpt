--TEST--
mysqli_execute_query() does not leak stmt->query on input_params validation errors with MYSQLI_REPORT_INDEX
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require 'table.inc';

mysqli_report(MYSQLI_REPORT_INDEX);

try {
    $link->execute_query('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?', ['foo', 42]);
} catch (ValueError $e) {
    echo '[001] '.$e->getMessage()."\n";
}

try {
    $link->execute_query('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?', ['foo' => 42]);
} catch (ValueError $e) {
    echo '[002] '.$e->getMessage()."\n";
}

print "done!";
?>
--CLEAN--
<?php
require_once 'clean_table.inc';
?>
--EXPECT--
[001] mysqli::execute_query(): Argument #2 ($params) must consist of exactly 3 elements, 2 present
[002] mysqli::execute_query(): Argument #2 ($params) must be a list array
done!
