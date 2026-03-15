--TEST--
Generic interface: implementation with bound type arguments
--FILE--
<?php
interface Repository<T> {
    public function find(int $id): T;
    public function save(T $entity): void;
}

class UserRepo implements Repository<string> {
    private array $store = [];
    public function find(int $id): string { return $this->store[$id] ?? "unknown"; }
    public function save(string $entity): void {
        $this->store[] = $entity;
        echo "Saved: $entity\n";
    }
}

$repo = new UserRepo();
$repo->save("alice");
echo $repo->find(0) . "\n";
echo "OK\n";
?>
--EXPECT--
Saved: alice
alice
OK
