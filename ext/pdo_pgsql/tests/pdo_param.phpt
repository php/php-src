--TEST--
PDO PgSQL PDOStatement PDO::PARAM_XXX
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

// The LOB test is omitted because it exists separately.
foreach ([
    'PDO::PARAM_STR',
    'PDO::PARAM_INT',
    'PDO::PARAM_BOOL',
    'PDO::PARAM_NULL',
] as $param_type_const) {
    foreach ([
        null,
        0,
        8,
        '',
        '0',
        '10',
        'str',
        true,
        false,
    ] as $val) {
        foreach ([1, 0] as $emulate) {
            printf('%s, var = %s, PDO::ATTR_EMULATE_PREPARES = %d: ', $param_type_const, var_export($val, true), $emulate);
            $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, $emulate);
            $stmt = $db->prepare('SELECT :val as val');
            $stmt->bindValue(':val', $val, constant($param_type_const));
            $stmt->execute();
            $row = $stmt->fetch(PDO::FETCH_ASSOC);
            var_dump($row['val']);
        }
    }
    if ($param_type_const !== 'PDO::PARAM_NULL') {
        echo PHP_EOL;
    }
}
?>
--EXPECT--
PDO::PARAM_STR, var = NULL, PDO::ATTR_EMULATE_PREPARES = 1: NULL
PDO::PARAM_STR, var = NULL, PDO::ATTR_EMULATE_PREPARES = 0: NULL
PDO::PARAM_STR, var = 0, PDO::ATTR_EMULATE_PREPARES = 1: string(1) "0"
PDO::PARAM_STR, var = 0, PDO::ATTR_EMULATE_PREPARES = 0: string(1) "0"
PDO::PARAM_STR, var = 8, PDO::ATTR_EMULATE_PREPARES = 1: string(1) "8"
PDO::PARAM_STR, var = 8, PDO::ATTR_EMULATE_PREPARES = 0: string(1) "8"
PDO::PARAM_STR, var = '', PDO::ATTR_EMULATE_PREPARES = 1: string(0) ""
PDO::PARAM_STR, var = '', PDO::ATTR_EMULATE_PREPARES = 0: string(0) ""
PDO::PARAM_STR, var = '0', PDO::ATTR_EMULATE_PREPARES = 1: string(1) "0"
PDO::PARAM_STR, var = '0', PDO::ATTR_EMULATE_PREPARES = 0: string(1) "0"
PDO::PARAM_STR, var = '10', PDO::ATTR_EMULATE_PREPARES = 1: string(2) "10"
PDO::PARAM_STR, var = '10', PDO::ATTR_EMULATE_PREPARES = 0: string(2) "10"
PDO::PARAM_STR, var = 'str', PDO::ATTR_EMULATE_PREPARES = 1: string(3) "str"
PDO::PARAM_STR, var = 'str', PDO::ATTR_EMULATE_PREPARES = 0: string(3) "str"
PDO::PARAM_STR, var = true, PDO::ATTR_EMULATE_PREPARES = 1: string(1) "1"
PDO::PARAM_STR, var = true, PDO::ATTR_EMULATE_PREPARES = 0: string(1) "1"
PDO::PARAM_STR, var = false, PDO::ATTR_EMULATE_PREPARES = 1: string(0) ""
PDO::PARAM_STR, var = false, PDO::ATTR_EMULATE_PREPARES = 0: string(0) ""

PDO::PARAM_INT, var = NULL, PDO::ATTR_EMULATE_PREPARES = 1: NULL
PDO::PARAM_INT, var = NULL, PDO::ATTR_EMULATE_PREPARES = 0: NULL
PDO::PARAM_INT, var = 0, PDO::ATTR_EMULATE_PREPARES = 1: int(0)
PDO::PARAM_INT, var = 0, PDO::ATTR_EMULATE_PREPARES = 0: int(0)
PDO::PARAM_INT, var = 8, PDO::ATTR_EMULATE_PREPARES = 1: int(8)
PDO::PARAM_INT, var = 8, PDO::ATTR_EMULATE_PREPARES = 0: int(8)
PDO::PARAM_INT, var = '', PDO::ATTR_EMULATE_PREPARES = 1: int(0)
PDO::PARAM_INT, var = '', PDO::ATTR_EMULATE_PREPARES = 0: int(0)
PDO::PARAM_INT, var = '0', PDO::ATTR_EMULATE_PREPARES = 1: int(0)
PDO::PARAM_INT, var = '0', PDO::ATTR_EMULATE_PREPARES = 0: int(0)
PDO::PARAM_INT, var = '10', PDO::ATTR_EMULATE_PREPARES = 1: int(10)
PDO::PARAM_INT, var = '10', PDO::ATTR_EMULATE_PREPARES = 0: int(10)
PDO::PARAM_INT, var = 'str', PDO::ATTR_EMULATE_PREPARES = 1: int(0)
PDO::PARAM_INT, var = 'str', PDO::ATTR_EMULATE_PREPARES = 0: int(0)
PDO::PARAM_INT, var = true, PDO::ATTR_EMULATE_PREPARES = 1: int(1)
PDO::PARAM_INT, var = true, PDO::ATTR_EMULATE_PREPARES = 0: int(1)
PDO::PARAM_INT, var = false, PDO::ATTR_EMULATE_PREPARES = 1: int(0)
PDO::PARAM_INT, var = false, PDO::ATTR_EMULATE_PREPARES = 0: int(0)

PDO::PARAM_BOOL, var = NULL, PDO::ATTR_EMULATE_PREPARES = 1: NULL
PDO::PARAM_BOOL, var = NULL, PDO::ATTR_EMULATE_PREPARES = 0: NULL
PDO::PARAM_BOOL, var = 0, PDO::ATTR_EMULATE_PREPARES = 1: string(1) "f"
PDO::PARAM_BOOL, var = 0, PDO::ATTR_EMULATE_PREPARES = 0: string(1) "f"
PDO::PARAM_BOOL, var = 8, PDO::ATTR_EMULATE_PREPARES = 1: string(1) "t"
PDO::PARAM_BOOL, var = 8, PDO::ATTR_EMULATE_PREPARES = 0: string(1) "t"
PDO::PARAM_BOOL, var = '', PDO::ATTR_EMULATE_PREPARES = 1: string(1) "f"
PDO::PARAM_BOOL, var = '', PDO::ATTR_EMULATE_PREPARES = 0: string(1) "f"
PDO::PARAM_BOOL, var = '0', PDO::ATTR_EMULATE_PREPARES = 1: string(1) "f"
PDO::PARAM_BOOL, var = '0', PDO::ATTR_EMULATE_PREPARES = 0: string(1) "f"
PDO::PARAM_BOOL, var = '10', PDO::ATTR_EMULATE_PREPARES = 1: string(1) "t"
PDO::PARAM_BOOL, var = '10', PDO::ATTR_EMULATE_PREPARES = 0: string(1) "t"
PDO::PARAM_BOOL, var = 'str', PDO::ATTR_EMULATE_PREPARES = 1: string(1) "t"
PDO::PARAM_BOOL, var = 'str', PDO::ATTR_EMULATE_PREPARES = 0: string(1) "t"
PDO::PARAM_BOOL, var = true, PDO::ATTR_EMULATE_PREPARES = 1: string(1) "t"
PDO::PARAM_BOOL, var = true, PDO::ATTR_EMULATE_PREPARES = 0: string(1) "t"
PDO::PARAM_BOOL, var = false, PDO::ATTR_EMULATE_PREPARES = 1: string(1) "f"
PDO::PARAM_BOOL, var = false, PDO::ATTR_EMULATE_PREPARES = 0: string(1) "f"

PDO::PARAM_NULL, var = NULL, PDO::ATTR_EMULATE_PREPARES = 1: NULL
PDO::PARAM_NULL, var = NULL, PDO::ATTR_EMULATE_PREPARES = 0: NULL
PDO::PARAM_NULL, var = 0, PDO::ATTR_EMULATE_PREPARES = 1: NULL
PDO::PARAM_NULL, var = 0, PDO::ATTR_EMULATE_PREPARES = 0: NULL
PDO::PARAM_NULL, var = 8, PDO::ATTR_EMULATE_PREPARES = 1: NULL
PDO::PARAM_NULL, var = 8, PDO::ATTR_EMULATE_PREPARES = 0: NULL
PDO::PARAM_NULL, var = '', PDO::ATTR_EMULATE_PREPARES = 1: NULL
PDO::PARAM_NULL, var = '', PDO::ATTR_EMULATE_PREPARES = 0: NULL
PDO::PARAM_NULL, var = '0', PDO::ATTR_EMULATE_PREPARES = 1: NULL
PDO::PARAM_NULL, var = '0', PDO::ATTR_EMULATE_PREPARES = 0: NULL
PDO::PARAM_NULL, var = '10', PDO::ATTR_EMULATE_PREPARES = 1: NULL
PDO::PARAM_NULL, var = '10', PDO::ATTR_EMULATE_PREPARES = 0: NULL
PDO::PARAM_NULL, var = 'str', PDO::ATTR_EMULATE_PREPARES = 1: NULL
PDO::PARAM_NULL, var = 'str', PDO::ATTR_EMULATE_PREPARES = 0: NULL
PDO::PARAM_NULL, var = true, PDO::ATTR_EMULATE_PREPARES = 1: NULL
PDO::PARAM_NULL, var = true, PDO::ATTR_EMULATE_PREPARES = 0: NULL
PDO::PARAM_NULL, var = false, PDO::ATTR_EMULATE_PREPARES = 1: NULL
PDO::PARAM_NULL, var = false, PDO::ATTR_EMULATE_PREPARES = 0: NULL
