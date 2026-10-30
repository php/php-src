--TEST--
PgSQL PDO Parser custom syntax
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$query = <<<'EOF'
SELECT e'He''ll\'o? ' || ? AS b -- '
UNION ALL
SELECT 'He''ll''o?\' || ? AS b -- '
UNION ALL
SELECT U&'d\0061t\+000061? ' || ? AS b /* :name */
UNION ALL
SELECT $__$Is this a $$dollar$$ 'escaping'? $__$ || ? AS b -- ?
UNION ALL
SELECT $$Escaped question mark here?? $$ || ? AS b -- ?
EOF;

$stmt = $db->prepare($query);
$stmt->execute(['World', 'World', 'base', 'Yes', 'Yes']);

while ($row = $stmt->fetchColumn()) {
    var_dump($row);
}

// Nested comments are incompatible: PDO parses the "?" inside the comment, Postgres doesn't
$query = <<<'EOF'
SELECT 'Hello' || ? /* is this a /* nested */ 'comment' ? */
EOF;

$stmt = $db->prepare($query);

try {
    $stmt->execute(['X', 'Y', 'Z']);
} catch (PDOException $e) {
    // PDO error: 3 parameters vs 2 expected
    var_dump('HY093' === $e->getCode());
}

try {
    $stmt->execute(['X', 'Y']);
} catch (PDOException $e) {
    // PgSQL error: Indeterminate datatype (1 extra parameter)
    var_dump('42P18' === $e->getCode());
}

?>
--EXPECTF--
Deprecated: PDO::prepare(): Escaping question marks inside dollar quoted strings is not required anymore and is deprecated in %s on line %d
string(14) "He'll'o? World"
string(14) "He'll'o?\World"
string(10) "data? base"
string(36) "Is this a $$dollar$$ 'escaping'? Yes"
string(31) "Escaped question mark here? Yes"
bool(true)
bool(true)
