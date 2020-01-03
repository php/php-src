<?php
require_once dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
require_once dirname(__FILE__) . '/config.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');

attach($db);

$db->beginTransaction();
$db->exec("create temporary table t (a varchar(3))");
$db->exec("create function hey() returns trigger as \$\$ begin new.a := 'oh'; raise notice 'I tampered your data, did you know?'; return new; end; \$\$ language plpgsql");
$db->exec("create trigger hop before insert on t for each row execute procedure hey()");
$db->exec("insert into t values ('ah')");
attach($db, 'Re');
$db->exec("delete from t");
$db->exec("insert into t values ('ah')");
$db->pgsqlSetNoticeCallback(null);
$db->exec("delete from t");
$db->exec("insert into t values ('ah')");
var_dump($db->query("select * from t")->fetchAll(PDO::FETCH_ASSOC));
echo "Done\n";
?>
