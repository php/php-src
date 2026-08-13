--TEST--
Rebinding or re-executing from a parameter __toString() must not mutate bound_params mid-FOREACH
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
class Rebind {
    public function __construct(private PDOStatement $stmt) {}
    public function __toString() {
        try {
            $this->stmt->bindValue(1, 'x');
            echo "bindValue: no error\n";
        } catch (Error $e) {
            echo $e::class, ": ", $e->getMessage(), "\n";
        }
        return 'rebind';
    }
}

class Reexec {
    public function __construct(private PDOStatement $stmt) {}
    public function __toString() {
        try {
            $this->stmt->execute(['x', 'y']);
            echo "execute: no error\n";
        } catch (Error $e) {
            echo $e::class, ": ", $e->getMessage(), "\n";
        }
        return 'reexec';
    }
}

class Reclose {
    public function __construct(private PDOStatement $stmt) {}
    public function __toString() {
        try {
            $this->stmt->closeCursor();
            echo "closeCursor: no error\n";
        } catch (Error $e) {
            echo $e::class, ": ", $e->getMessage(), "\n";
        }
        return 'reclose';
    }
}

$db = new PDO('sqlite::memory:');

echo "bindValue:\n";
$stmt = $db->prepare('SELECT ?, ?');
$p1 = 'placeholder';
$p2 = 'second';
$stmt->bindParam(1, $p1);
$stmt->bindParam(2, $p2);
$p1 = new Rebind($stmt);
try {
    $stmt->execute();
    echo "execute after bindValue: no error\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

echo "execute:\n";
$stmt = $db->prepare('SELECT ?, ?');
$p1 = 'placeholder';
$p2 = 'second';
$stmt->bindParam(1, $p1);
$stmt->bindParam(2, $p2);
$p1 = new Reexec($stmt);
try {
    $stmt->execute();
    echo "execute after execute: no error\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

echo "closeCursor:\n";
$stmt = $db->prepare('SELECT ?, ?');
$p1 = 'placeholder';
$p2 = 'second';
$stmt->bindParam(1, $p1);
$stmt->bindParam(2, $p2);
$p1 = new Reclose($stmt);
try {
    $stmt->execute();
    echo "execute after closeCursor: no error\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

echo "done\n";
?>
--EXPECT--
bindValue:
Error: Cannot modify a PDOStatement while parameter hooks are running
execute after bindValue: no error
execute:
Error: Cannot modify a PDOStatement while parameter hooks are running
execute after execute: no error
closeCursor:
Error: Cannot modify a PDOStatement while parameter hooks are running
execute after closeCursor: no error
done
