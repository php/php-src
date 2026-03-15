--TEST--
Generic class: JIT + covariant/contravariant type checks
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
--FILE--
<?php
declare(strict_types=1);

class Animal {}
class Dog extends Animal {}
class Cat extends Animal {}

// Covariant: out T — only in return positions
class ReadOnlyBox<out T> {
    private mixed $value;
    public function __construct(mixed $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

// Contravariant: in T — only in parameter positions
class WriteOnlyBox<in T> {
    private mixed $value;
    public function set(T $value): void { $this->value = $value; }
    public function dump(): void { echo get_class($this->value) . "\n"; }
}

// 1. Covariant basic
$dogBox = new ReadOnlyBox<Dog>(new Dog());
echo "1. " . get_class($dogBox->get()) . "\n";

// 2. Contravariant basic
$animalBox = new WriteOnlyBox<Animal>(null);
$animalBox->set(new Dog());
echo "2. OK\n";
$animalBox->dump();

// 3. Hot loop with covariant
for ($i = 0; $i < 200; $i++) {
    $rb = new ReadOnlyBox<Dog>(new Dog());
    $rb->get();
}
echo "3. hot covariant OK\n";

// 4. Hot loop with contravariant
for ($i = 0; $i < 200; $i++) {
    $wb = new WriteOnlyBox<Animal>(null);
    $wb->set(new Cat());
}
echo "4. hot contravariant OK\n";

// 5. instanceof with covariant
$rb = new ReadOnlyBox<Dog>(new Dog());
echo "5. instanceof: " . ($rb instanceof ReadOnlyBox ? "yes" : "no") . "\n";

echo "Done.\n";
?>
--EXPECT--
1. Dog
2. OK
Dog
3. hot covariant OK
4. hot contravariant OK
5. instanceof: yes
Done.
