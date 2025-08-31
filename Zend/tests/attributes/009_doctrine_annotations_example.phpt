--TEST--
Doctrine-like attributes usage
--FILE--
<?php

namespace Doctrine\ORM\Attributes {
    class Annotation { public $values; public function construct() { $this->values = func_get_args(); } }
    class Entity extends Annotation {}
    class Id extends Annotation {}
    class Column extends Annotation { const UNIQUE = 'unique'; const T_INTEGER = 'integer'; }
    class GeneratedValue extends Annotation {}
    class JoinTable extends Annotation {}
    class ManyToMany extends Annotation {}
    class JoinColumn extends Annotation { const UNIQUE = 'unique'; }
    class InverseJoinColumn extends Annotation {}
}

namespace Symfony\Component\Validator\Constraints {
    class Annotation { public $values; public function construct() { $this->values = func_get_args(); } }
    class Email extends Annotation {}
    class Range extends Annotation {}
}

namespace {
use Doctrine\ORM\Attributes as ORM;
use Symfony\Component\Validator\Constraints as Assert;

#[ORM\Entity]
/** @ORM\Entity */
class User
{
    /** @ORM\Id @ORM\Column(type="integer"*) @ORM\GeneratedValue */
    #[ORM\Id]
    #[ORM\Column("integer")]
    #[ORM\GeneratedValue]
    private $id;

    /**
     * @ORM\Column(type="string", unique=true)
     * @Assert\Email(message="The email '{{ value }}' is not a valid email.")
     */
    #[ORM\Column("string", ORM\Column::UNIQUE)]
    #[Assert\Email(array("message" => "The email '{{ value }}' is not a valid email."))]
    private $email;

    /**
     * @ORM\Column(type="integer")
     * @Assert\Range(
     *      min = 120,
     *      max = 180,
     *      minMessage = "You must be at least {{ limit }}cm tall to enter",
     *      maxMessage = "You cannot be taller than {{ limit }}cm to enter"
     * )
     */
    #[Assert\Range(["min" => 120, "max" => 180, "minMessage" => "You must be at least {{ limit }}cm tall to enter"])]
    #[ORM\Column(ORM\Column::T_INTEGER)]
    protected $height;

    /**
     * @ORM\ManyToMany(targetEntity="Phonenumber")
     * @ORM\JoinTable(name="users_phonenumbers",
     *      joinColumns={@ORM\JoinColumn(name="user_id", referencedColumnName="id")},
     *      inverseJoinColumns={@ORM\JoinColumn(name="phonenumber_id", referencedColumnName="id", unique=true)}
     *      )
     */
    #[ORM\ManyToMany(Phonenumber::class)]
    #[ORM\JoinTable("users_phonenumbers")]
    #[ORM\JoinColumn("user_id", "id")]
    #[ORM\InverseJoinColumn("phonenumber_id", "id", ORM\JoinColumn::UNIQUE)]
    private $phonenumbers;
}

$class = new ReflectionClass(User::class);
$attributes = $class->getAttributes();

foreach ($attributes as $attribute) {
    var_dump($attribute->getName(), $attribute->getArguments());
}

foreach ($class->getProperties() as $property) {
    $attributes = $property->getAttributes();

    foreach ($attributes as $attribute) {
        var_dump($attribute->getName(), $attribute->getArguments());
    }
}
}
?>
--EXPECT--
string(30) "Doctrine\ORM\Attributes\Entity"
array(0) {
}
string(26) "Doctrine\ORM\Attributes\Id"
array(0) {
}
string(30) "Doctrine\ORM\Attributes\Column"
array(1) {
  [0]=>
  string(7) "integer"
}
string(38) "Doctrine\ORM\Attributes\GeneratedValue"
array(0) {
}
string(30) "Doctrine\ORM\Attributes\Column"
array(2) {
  [0]=>
  string(6) "string"
  [1]=>
  string(6) "unique"
}
string(45) "Symfony\Component\Validator\Constraints\Email"
array(1) {
  [0]=>
  array(1) {
    ["message"]=>
    string(45) "The email '{{ value }}' is not a valid email."
  }
}
string(45) "Symfony\Component\Validator\Constraints\Range"
array(1) {
  [0]=>
  array(3) {
    ["min"]=>
    int(120)
    ["max"]=>
    int(180)
    ["minMessage"]=>
    string(48) "You must be at least {{ limit }}cm tall to enter"
  }
}
string(30) "Doctrine\ORM\Attributes\Column"
array(1) {
  [0]=>
  string(7) "integer"
}
string(34) "Doctrine\ORM\Attributes\ManyToMany"
array(1) {
  [0]=>
  string(11) "Phonenumber"
}
string(33) "Doctrine\ORM\Attributes\JoinTable"
array(1) {
  [0]=>
  string(18) "users_phonenumbers"
}
string(34) "Doctrine\ORM\Attributes\JoinColumn"
array(2) {
  [0]=>
  string(7) "user_id"
  [1]=>
  string(2) "id"
}
string(41) "Doctrine\ORM\Attributes\InverseJoinColumn"
array(3) {
  [0]=>
  string(14) "phonenumber_id"
  [1]=>
  string(2) "id"
  [2]=>
  string(6) "unique"
}
