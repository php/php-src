--TEST--
Lazy objects: RFC example 011
--FILE--
<?php

// User code

class BlogPost
{
    private int $id;
    private string $name;
    private string $email;

    public function getName()
    {
        return $this->name;
    }

    public function getEmail()
    {
        return $this->email;
    }
}

// ORM code

class EntityManager
{
    public function getReference(string $class, int $id)
    {
        // The ReflectionClass and ReflectionProperty instances are cached in practice
        $reflector = new ReflectionClass($class);

        $entity = $reflector->newLazyGhost(function ($entity) use ($class, $id, $reflector) {
            $data = $this->loadFromDatabase($class, $id);
            $reflector->getProperty('name')->setValue($entity, $data['name']);
            $reflector->getProperty('email')->setValue($entity, $data['email']);
        });

        // id is already known and can be accessed without triggering initialization
        $reflector->getProperty('id')->setRawValueWithoutLazyInitialization($entity, $id);

        return $entity;
    }

    public function loadFromDatabase($id)
    {
        return [
            'name' => 'Example',
            'email' => 'example@example.com',
        ];
    }
}

$em = new EntityManager();
$blogPost = $em->getReference(BlogPost::class, 123);
var_dump($blogPost);
var_dump($blogPost->getName());
var_dump($blogPost);

?>
==DONE==
--EXPECTF--
lazy ghost object(BlogPost)#%d (1) {
  ["id":"BlogPost":private]=>
  int(123)
  ["name":"BlogPost":private]=>
  uninitialized(string)
  ["email":"BlogPost":private]=>
  uninitialized(string)
}
string(7) "Example"
object(BlogPost)#%d (3) {
  ["id":"BlogPost":private]=>
  int(123)
  ["name":"BlogPost":private]=>
  string(7) "Example"
  ["email":"BlogPost":private]=>
  string(19) "example@example.com"
}
==DONE==
