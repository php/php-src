--TEST--
PostgreSQL pg_meta_data() - basic test using schema
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

include('config.inc');

$conn = pg_connect($conn_str);

pg_query('CREATE SCHEMA phptests');

pg_query('CREATE TABLE phptests.foo (id INT, id2 INT)');

pg_query('CREATE TABLE foo (id INT, id3 INT)');


var_dump(pg_meta_data($conn, 'foo'));
var_dump(pg_meta_data($conn, 'phptests.foo'));


pg_query('DROP TABLE foo');
pg_query('DROP TABLE phptests.foo');
pg_query('DROP SCHEMA phptests');

?>
--EXPECT--
array(2) {
  [u"id"]=>
  array(6) {
    [u"num"]=>
    int(1)
    [u"type"]=>
    unicode(4) "int4"
    [u"len"]=>
    int(4)
    [u"not null"]=>
    bool(false)
    [u"has default"]=>
    bool(false)
    [u"array dims"]=>
    int(0)
  }
  [u"id3"]=>
  array(6) {
    [u"num"]=>
    int(2)
    [u"type"]=>
    unicode(4) "int4"
    [u"len"]=>
    int(4)
    [u"not null"]=>
    bool(false)
    [u"has default"]=>
    bool(false)
    [u"array dims"]=>
    int(0)
  }
}
array(2) {
  [u"id"]=>
  array(6) {
    [u"num"]=>
    int(1)
    [u"type"]=>
    unicode(4) "int4"
    [u"len"]=>
    int(4)
    [u"not null"]=>
    bool(false)
    [u"has default"]=>
    bool(false)
    [u"array dims"]=>
    int(0)
  }
  [u"id2"]=>
  array(6) {
    [u"num"]=>
    int(2)
    [u"type"]=>
    unicode(4) "int4"
    [u"len"]=>
    int(4)
    [u"not null"]=>
    bool(false)
    [u"has default"]=>
    bool(false)
    [u"array dims"]=>
    int(0)
  }
}
