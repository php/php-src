--TEST--
GC must not free callbacks of a persistent connection shared by several PDO objects
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
class Repo {
    public Pdo\Sqlite $pdo;

    public function __construct(public string $name) {
        $this->pdo = Pdo\Sqlite::connect('sqlite::memory:', null, null, [PDO::ATTR_PERSISTENT => true]);
        $this->pdo->createFunction("udf_$name", [$this, 'udf'], 1);
    }

    public function udf($x) {
        return $this->name . $x;
    }
}

$repos = [new Repo('one'), new Repo('two')];
echo "collected: "; var_dump(gc_collect_cycles());
echo "udf_one: "; var_dump($repos[0]->pdo->query("SELECT udf_one('!')")->fetchColumn());
echo "udf_two: "; var_dump($repos[1]->pdo->query("SELECT udf_two('?')")->fetchColumn());

unset($repos[0]->pdo, $repos[1]->pdo);
?>
--EXPECT--
collected: int(0)
udf_one: string(4) "one!"
udf_two: string(4) "two?"
