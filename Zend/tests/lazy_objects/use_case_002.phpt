--TEST--
Lazy objects: Lazy entity loading
--FILE--
<?php

class User {
    private string $transient;

    public function __construct(
        private int $id,
        private string $name,
    ) {
    }

    public function getId(): int {
        return $this->id;
    }

    public function getName(): string {
        return $this->name;
    }
}

class EntityManager {
    public function lazyLoad(string $fqcn, int $id): object {
        $reflector = new ReflectionClass($fqcn);
        $entity = $reflector->newLazyGhost(function ($obj) {
            var_dump('initializer');
            $prop = new ReflectionProperty($obj::class, 'name');
            $prop->setValue($obj, 'John Doe');
        });

        (new ReflectionProperty($fqcn, 'id'))->setRawValueWithoutLazyInitialization($entity, $id);

        return $entity;
    }
}

$em = new EntityManager();
$user = $em->lazyLoad(User::class, 123);

printf("Fetching identifier does not trigger initializer\n");
var_dump($user->getId());

printf("Fetching anything else triggers initializer\n");
var_dump($user->getName());
--EXPECTF--
Fetching identifier does not trigger initializer
int(123)
Fetching anything else triggers initializer
string(11) "initializer"
string(8) "John Doe"
