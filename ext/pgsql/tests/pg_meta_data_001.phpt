--TEST--
PostgreSQL pg_meta_data() - basic test using schema
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

include('config.inc');

$conn = pg_connect($conn_str);

pg_query($conn, 'CREATE SCHEMA phptests');

pg_query($conn, 'CREATE TABLE phptests.foo (id INT, id2 INT)');

pg_query($conn, 'CREATE TABLE foo (id INT, id3 INT)');


var_dump(pg_meta_data($conn, 'foo'));
var_dump(pg_meta_data($conn, 'phptests.foo'));
var_dump(pg_meta_data($conn, 'phptests.foo', TRUE));
var_dump(pg_meta_data($conn, 'phptests.foo'));
var_dump(pg_meta_data($conn, 'phptests.foo', TRUE));


pg_query($conn, 'DROP TABLE foo');
pg_query($conn, 'DROP TABLE phptests.foo');
pg_query($conn, 'DROP SCHEMA phptests');

?>
--EXPECT--
array(3) {
  ["id"]=>
  array(7) {
    ["num"]=>
    int(1)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
    ["array dims"]=>
    int(0)
    ["is enum"]=>
    bool(false)
  }
  ["id3"]=>
  array(7) {
    ["num"]=>
    int(2)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
    ["array dims"]=>
    int(0)
    ["is enum"]=>
    bool(false)
  }
  ["pg_meta_data_cached"]=>
  bool(false)
}
array(3) {
  ["id"]=>
  array(7) {
    ["num"]=>
    int(1)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
    ["array dims"]=>
    int(0)
    ["is enum"]=>
    bool(false)
  }
  ["id2"]=>
  array(7) {
    ["num"]=>
    int(2)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
    ["array dims"]=>
    int(0)
    ["is enum"]=>
    bool(false)
  }
  ["pg_meta_data_cached"]=>
  bool(false)
}
array(3) {
  ["id"]=>
  array(11) {
    ["num"]=>
    int(1)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
    ["array dims"]=>
    int(0)
    ["is enum"]=>
    bool(false)
    ["is base"]=>
    bool(true)
    ["is composite"]=>
    bool(false)
    ["is pseudo"]=>
    bool(false)
    ["description"]=>
    string(0) ""
  }
  ["id2"]=>
  array(11) {
    ["num"]=>
    int(2)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
    ["array dims"]=>
    int(0)
    ["is enum"]=>
    bool(false)
    ["is base"]=>
    bool(true)
    ["is composite"]=>
    bool(false)
    ["is pseudo"]=>
    bool(false)
    ["description"]=>
    string(0) ""
  }
  ["pg_meta_data_cached"]=>
  bool(false)
}
array(3) {
  ["id"]=>
  array(7) {
    ["num"]=>
    int(1)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
    ["array dims"]=>
    int(0)
    ["is enum"]=>
    bool(false)
  }
  ["id2"]=>
  array(7) {
    ["num"]=>
    int(2)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
    ["array dims"]=>
    int(0)
    ["is enum"]=>
    bool(false)
  }
  ["pg_meta_data_cached"]=>
  bool(true)
}
array(3) {
  ["id"]=>
  array(11) {
    ["num"]=>
    int(1)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
    ["array dims"]=>
    int(0)
    ["is enum"]=>
    bool(false)
    ["is base"]=>
    bool(true)
    ["is composite"]=>
    bool(false)
    ["is pseudo"]=>
    bool(false)
    ["description"]=>
    string(0) ""
  }
  ["id2"]=>
  array(11) {
    ["num"]=>
    int(2)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
    ["array dims"]=>
    int(0)
    ["is enum"]=>
    bool(false)
    ["is base"]=>
    bool(true)
    ["is composite"]=>
    bool(false)
    ["is pseudo"]=>
    bool(false)
    ["description"]=>
    string(0) ""
  }
  ["pg_meta_data_cached"]=>
  bool(true)
}
[Sat May  6 22:56:16 2023]  Script:  '/home/dcarlier/Contribs/php-src/ext/pgsql/tests/pg_meta_data_001.php'
/home/dcarlier/Contribs/php-src/Zend/zend_string.h(174) :  Freeing 0x00007fa198001a40 (32 bytes), script=/home/dcarlier/Contribs/php-src/ext/pgsql/tests/pg_meta_data_001.php
Last leak repeated 16 times
[Sat May  6 22:56:16 2023]  Script:  '/home/dcarlier/Contribs/php-src/ext/pgsql/tests/pg_meta_data_001.php'
/home/dcarlier/Contribs/php-src/Zend/zend_string.h(174) :  Freeing 0x00007fa198001b80 (32 bytes), script=/home/dcarlier/Contribs/php-src/ext/pgsql/tests/pg_meta_data_001.php
Last leak repeated 1 time
[Sat May  6 22:56:16 2023]  Script:  '/home/dcarlier/Contribs/php-src/ext/pgsql/tests/pg_meta_data_001.php'
/home/dcarlier/Contribs/php-src/Zend/zend_hash.c(291) :  Freeing 0x00007fa19805e660 (56 bytes), script=/home/dcarlier/Contribs/php-src/ext/pgsql/tests/pg_meta_data_001.php
Last leak repeated 2 times
[Sat May  6 22:56:16 2023]  Script:  '/home/dcarlier/Contribs/php-src/ext/pgsql/tests/pg_meta_data_001.php'
/home/dcarlier/Contribs/php-src/Zend/zend_hash.c(177) :  Freeing 0x00007fa19806e300 (320 bytes), script=/home/dcarlier/Contribs/php-src/ext/pgsql/tests/pg_meta_data_001.php
Last leak repeated 2 times
=== Total 25 memory leaks detected ===
