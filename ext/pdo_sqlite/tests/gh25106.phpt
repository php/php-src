--TEST--
GH-25106: PDO SQLite discards unread rows before re-executing a statement
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$db = new PDO('sqlite::memory:');

foreach (['fresh', 'unread', 'closed', 'consumed'] as $state) {
    $stmt = $db->prepare('SELECT 42 AS answer WHERE ?');
    if ($state !== 'fresh') {
        $stmt->execute([1]);
        if ($state === 'closed') {
            $stmt->closeCursor();
        } elseif ($state === 'consumed') {
            $stmt->fetchAll(PDO::FETCH_ASSOC);
        }
    }

    $stmt->execute([0]);
    echo "$state empty: ", json_encode($stmt->fetchAll(PDO::FETCH_ASSOC)), "\n";
    $stmt->execute([1]);
    echo "$state row: ", json_encode($stmt->fetchAll(PDO::FETCH_ASSOC)), "\n";
}
?>
--EXPECT--
fresh empty: []
fresh row: [{"answer":42}]
unread empty: []
unread row: [{"answer":42}]
closed empty: []
closed row: [{"answer":42}]
consumed empty: []
consumed row: [{"answer":42}]
