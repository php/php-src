--TEST--
Bug #73423 (Reproducible crash with GDB backtrace)
--FILE--
<?php

class foo implements \RecursiveIterator
  {
    public $foo = [];

    public Function current(): mixed
      {
        return current ($this->foo);
      }

    public Function key(): mixed
      {
        return key ($this->foo);
      }

    public Function next(): void
      {
        next ($this->foo);
      }

    public Function rewind(): void
      {
        reset ($this->foo);
      }

    public Function valid(): bool
      {
        return current ($this->foo) !== false;
      }

    public Function getChildren(): ?RecursiveIterator
      {
        return current ($this->foo);
      }

    public Function hasChildren(): bool
      {
        return (bool) count($this->foo);
      }
  }


class fooIterator extends \RecursiveFilterIterator
  {
    public Function __destruct ()
      {
        eval("class A extends NotExists {}");

        /* CRASH */
      }

    public Function accept(): bool
      {
        return true;
      }
  }


$foo = new foo ();

$foo->foo[] = new foo ();

foreach (new \RecursiveIteratorIterator (new fooIterator ($foo)) as $bar) ;

?>
--EXPECTF--
Fatal error: Uncaught Error: Class "NotExists" not found in %s:%d
Stack trace:
#0 %s(%d): eval()
#1 %s(%d): fooIterator->__destruct()
#2 {main}

Next Error: Class "NotExists" not found in %s:%d
Stack trace:
#0 %s(%d): eval()
#1 %s(%d): fooIterator->__destruct()
#2 {main}
  thrown in %s on line %d
