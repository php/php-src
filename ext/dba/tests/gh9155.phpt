--TEST--
Bug GH-9155 (dba_open("non-existing", "c-", "flatfile") segfaults)
--SKIPIF--
<?php
$handler = "flatfile";
require_once(__DIR__ .'/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__ .'/test.inc');

$db = dba_open($db_filename, 'c-', 'flatfile');
var_dump($db);
?>
--CLEAN--
<?php
require_once(__DIR__ .'/clean.inc');
?>
--EXPECTF--
resource(%d) of type (dba)
