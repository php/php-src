--TEST--
ReflectionObject::__toString (filtering privates/protected dynamic properties)
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php
class C1 {
	private   $p1 = 1;
	protected $p2 = 2;
	public    $p3 = 3;
}

$x = new C1();
$x->z = 4;
$x->p3 = 5;

$obj = new ReflectionObject($x);
echo $obj;
?>
--EXPECTF--
Object of class [ <user> class C1 ] {
  @@ %s024.php 2-6

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [3] {
    Property [ <default> private $p1 ]
    Property [ <default> protected $p2 ]
    Property [ <default> public $p3 ]
  }

  - Dynamic properties [1] {
    Property [ <dynamic> public $z ]
  }

  - Methods [0] {
  }
}
