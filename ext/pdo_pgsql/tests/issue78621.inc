<?php
require_once dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
require_once dirname(__FILE__) . '/config.inc';
if (!isset($db)) {
    $db = new Pdo\Pgsql($config['ENV']['PDOTEST_DSN']);
}
if (!isset($rounds) || empty($rounds)) {
    $rounds = [ null, 'Re' ];
}

attach($db, array_shift($rounds));

$db->beginTransaction();
$db->exec("set client_min_messages to notice");
$db->exec("create temporary table t (a varchar(3))");
$db->exec("create function hey() returns trigger as \$\$ begin new.a := 'oh'; raise notice 'I tampered your data, did you know?'; return new; end; \$\$ language plpgsql");
$db->exec("create trigger hop before insert on t for each row execute procedure hey()");
$db->exec("insert into t values ('ah')");
while (count($rounds)) {
    try {
        attach($db, array_shift($rounds));
    } catch (Throwable $err) {
        echo "Caught ".get_class($err).": ".$err->getMessage()."\n";
    }
    try {
        $db->exec("delete from t");
        $db->exec("insert into t values ('ah')");
    } catch (Throwable $err) {
        echo "Caught ".get_class($err)." ".$err->getMessage()."\n";
    }
}
$db->setNoticeCallback(null);
$db->exec("delete from t");
$db->exec("insert into t values ('ah')");
var_dump($db->query("select * from t")->fetchAll(PDO::FETCH_ASSOC));
echo "Done\n";
$db->rollback();
?>
