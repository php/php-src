--TEST--
Output of properties indicates if they are abstract
--FILE--
<?php

abstract class Demo {
    abstract public $a { get; }
    public $b;
}

$class = new ReflectionClass(Demo::class);
echo $class;

$propA = new ReflectionProperty(Demo::class, 'a');
echo $propA;

$propB = new ReflectionProperty(Demo::class, 'b');
echo $propB;
?>
--EXPECTF--
Class [ <user> <iterateable> abstract class Demo ] {
  @@ %s %d-%d

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [2] {
    Property [ abstract public virtual $a { get; } ]
    Property [ public $b = NULL ]
  }

  - Methods [0] {
  }
}
Property [ abstract public virtual $a { get; } ]
Property [ public $b = NULL ]
