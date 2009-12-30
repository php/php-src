--TEST--
PostgreSQL pg_metadata()
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);

$meta = pg_meta_data($db, $table_name);

var_dump($meta);
?>
--EXPECT--
array(3) {
  [u"num"]=>
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
  [u"str"]=>
  array(6) {
    [u"num"]=>
    int(2)
    [u"type"]=>
    unicode(4) "text"
    [u"len"]=>
    int(-1)
    [u"not null"]=>
    bool(false)
    [u"has default"]=>
    bool(false)
    [u"array dims"]=>
    int(0)
  }
  [u"bin"]=>
  array(6) {
    [u"num"]=>
    int(3)
    [u"type"]=>
    unicode(5) "bytea"
    [u"len"]=>
    int(-1)
    [u"not null"]=>
    bool(false)
    [u"has default"]=>
    bool(false)
    [u"array dims"]=>
    int(0)
  }
}
