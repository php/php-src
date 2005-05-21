<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create', 'create table test (id int, name varchar(10))');
set_sql('insert', 'insert into test (id, name) values(0, :name)');
set_sql('select', 'select name from test where id=0');

$DB->query($SQL['create']);
$stmt = $DB->prepare($SQL['insert']);
$name = NULL;
$before_bind = $name;
$stmt->bindParam(':name', $name);
if ($name !== $before_bind) {
	echo "bind: fail\n";
} else {
	echo "bind: success\n";
}
var_dump($stmt->execute());
var_dump($DB->query($SQL['select'])->fetchColumn());
?>
