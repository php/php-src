--TEST--
PDO_Firebird: bug 73087 segfault binding blob parameter
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php
require 'testdb.inc';

$dbh->exec('recreate table atable (id integer not null, content blob sub_type 1 segment size 80)');
$S = $dbh->prepare('insert into atable (id, content) values (:id, :content)');
for ($I = 1; $I < 10; $I++) {
    $Params = [
        'id' => $I,
        'content' => base64_encode(random_bytes(10))
    ];
    foreach ($Params as $Param => $Value)
        $S->bindValue($Param, $Value);
    $S->execute();
}
unset($S);
unset($dbh);
echo 'OK';
?>
--EXPECT--
OK
