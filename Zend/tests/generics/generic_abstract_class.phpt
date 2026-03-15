--TEST--
Generic class: abstract generic class with abstract methods
--FILE--
<?php
declare(strict_types=1);

abstract class Repository<T> {
    abstract public function find(int $id): T;
    abstract public function save(T $entity): void;

    public function findAndSave(int $id): T {
        $item = $this->find($id);
        $this->save($item);
        return $item;
    }
}

class User {
    public function __construct(public int $id, public string $name) {}
}

class UserRepository extends Repository<User> {
    private array $store = [];

    public function find(int $id): User {
        return $this->store[$id] ?? new User($id, "User$id");
    }

    public function save(User $entity): void {
        $this->store[$entity->id] = $entity;
    }
}

// 1. Basic usage
$repo = new UserRepository();
$user = $repo->find(1);
echo "1. " . $user->name . "\n";

// 2. Save and retrieve
$repo->save(new User(2, "Alice"));
echo "2. " . $repo->find(2)->name . "\n";

// 3. Template method pattern
$u = $repo->findAndSave(3);
echo "3. " . $u->name . "\n";

// 4. Cannot instantiate abstract generic class
try {
    $r = new Repository<User>();
} catch (Error $e) {
    echo "4. Error: " . (str_contains($e->getMessage(), 'abstract') ? 'abstract' : $e->getMessage()) . "\n";
}

echo "Done.\n";
?>
--EXPECT--
1. User1
2. Alice
3. User3
4. Error: abstract
Done.
