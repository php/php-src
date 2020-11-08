--TEST--
Bug #73423 (Reproducible crash with GDB backtrace)
--FILE--
<?php

class foo implements \RecursiveIterator
  {
    public $foo = [];

    public Function current ()
      {
        return current ($this->foo);
      }

    public Function key ()
      {
        return key ($this->foo);
      }

    public Function next ()
      {
        next ($this->foo);
      }

    public Function rewind ()
      {
        reset ($this->foo);
      }

    public Function valid ()
      {
        return current ($this->foo) !== false;
      }

    public Function getChildren ()
      {
        return current ($this->foo);
      }

    public Function hasChildren ()
      {
        return (bool) count ($this->foo);
      }
  }


class fooIterator extends \RecursiveFilterIterator
  {
    public Function __destruct ()
      {
        eval("class A extends NotExists {}");

        /* CRASH */
      }

    public Function accept ()
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
