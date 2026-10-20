--TEST--
PgSQL PDO Parser: E'' strings next to text containing e/E
--EXTENSIONS--
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);

$queries = [
    // The "?" and ":name" inside the E'' strings must not be treated as placeholders
    ["SELECT E'here\\'s a ?' AS selected_e, ? AS value_e", ['one']],
    ["SELECT 'x'||e'\\'?' AS concat_e, :name AS reference_e", ['name' => 'two']],
    ["SELECT ('a%' LIKE 'a!%' ESCAPE'!')::text AS escape_e, E'\\\\:e' AS e, :e AS ee", ['e' => 'three']],
    ["SELECT date'2026-10-14' AS date_e, ?::text AS where_else", ['four']],
    ["SELECT ? AS e, ? AS E2 WHERE TRUE", ['five', 'six']],
    ["SELECT /* e'? */ :value AS value_e -- e'?", ['value' => 'seven']],
];

foreach ([true, false] as $emulate) {
    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, $emulate);
    echo $emulate ? "emulated:\n" : "native:\n";
    foreach ($queries as [$sql, $params]) {
        $stmt = $db->prepare($sql);
        $stmt->execute($params);
        echo json_encode($stmt->fetch()), "\n";
    }
}
?>
--EXPECT--
emulated:
{"selected_e":"here's a ?","value_e":"one"}
{"concat_e":"x'?","reference_e":"two"}
{"escape_e":"true","e":"\\:e","ee":"three"}
{"date_e":"2026-10-14","where_else":"four"}
{"e":"five","e2":"six"}
{"value_e":"seven"}
native:
{"selected_e":"here's a ?","value_e":"one"}
{"concat_e":"x'?","reference_e":"two"}
{"escape_e":"true","e":"\\:e","ee":"three"}
{"date_e":"2026-10-14","where_else":"four"}
{"e":"five","e2":"six"}
{"value_e":"seven"}
