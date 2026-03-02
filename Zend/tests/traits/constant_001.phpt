--TEST--
Trying to access a constant on Trait via a Class
--FILE--
<?php

trait Foo {
    public const PUBLIC = 'public';
    protected const PROTECTED = 'protected';
    private const PRIVATE = 'private';

    public function f1(): void {
        echo self::PUBLIC, ' via self', PHP_EOL;
        echo static::PUBLIC, ' via static', PHP_EOL;
        echo $this::PUBLIC, ' via $this', PHP_EOL;
    }
}

class Base {
    use Foo;

    public function f2(): void {
        echo self::PRIVATE, ' via self', PHP_EOL;
        echo static::PRIVATE, ' via static', PHP_EOL;
    }
}

class Derived extends Base {
    public function f3(): void {
        echo self::PROTECTED, ' via self', PHP_EOL;
        echo static::PROTECTED, ' via static', PHP_EOL;
        echo parent::PROTECTED, ' via parent', PHP_EOL;
    }
}

echo Base::PUBLIC, ' via class name', PHP_EOL;
echo (new Base)::PUBLIC, ' via object', PHP_EOL;
(new Base)->f1();
(new Base)->f2();
echo Derived::PUBLIC, ' via derived class name', PHP_EOL;
echo (new Derived)::PUBLIC, ' via derived class object', PHP_EOL;
(new Derived)->f3();
?>
--EXPECTF--
public via class name
public via object
public via self
public via static
public via $this
private via self
private via static
public via derived class name
public via derived class object
protected via self
protected via static
protected via parent
