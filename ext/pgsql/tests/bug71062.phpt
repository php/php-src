--TEST--
Bug #71062 pg_convert() doesn't accept ISO 8601 for datatype timestamp
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');

$db = pg_connect($conn_str);

$table = "public.test_table_bug71062_bug71062";

pg_query($db, "CREATE TABLE $table ( test_field TIMESTAMPTZ )");

// ISO 8601 (with 'T' between date and time)
$date_string_php_iso8601 = date_create('8 Dec 2015 5:38')->format(DateTime::ISO8601);

// ISO 8601 with the 'T' removed
$modified_format = 'Y-m-d H:i:sO';
$date_string_modified_iso8601 = date_create('8 Dec 2015 5:38')->format($modified_format);

printf("trying format %s \n", DateTime::ISO8601);
pg_convert($db, $table, ['test_field' => $date_string_php_iso8601]);

printf("trying format %s \n", $modified_format);
pg_convert($db, $table, ['test_field' => $date_string_modified_iso8601]);

print "done\n";

?>
==OK==
--CLEAN--
<?php
require_once('inc/config.inc');
$db = @pg_connect($conn_str);
$table = "public.test_table_bug71062_bug71062";

pg_query($db, "DROP TABLE IF EXISTS $table");
?>
--EXPECT--
trying format Y-m-d\TH:i:sO 
trying format Y-m-d H:i:sO 
done
==OK==
