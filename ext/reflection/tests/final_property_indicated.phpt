--TEST--
Output of properties indicates if they are final
--FILE--
<?php

class Demo {
    final public $a;
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
Class [ <user> class Demo ] {
  @@ %s %d-%d

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [2] {
    Property [ final public $a = NULL ]
    Property [ public $b = NULL ]
  }

  - Methods [0] {
  }
}
Property [ final public $a = NULL ]
Property [ public $b = NULL ]
