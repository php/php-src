--TEST--
PDO_Firebird: bug 73087 segfault binding blob parameter
--SKIPIF--
<?php if (!extension_loaded('interbase') || !extension_loaded('pdo_firebird')) die('skip'); ?>
--FILE--
<?php
require 'testdb.inc';
$C = new PDO('firebird:dbname='.$test_base, $user, $password) or die;
@$C->exec('drop table atable');
$C->exec('create table atable (id integer not null, content blob sub_type 1 segment size 80)');
$S = $C->prepare('insert into atable (id, content) values (:id, :content)');
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
unset($C);
echo 'OK';
?>
--EXPECT--
OK
