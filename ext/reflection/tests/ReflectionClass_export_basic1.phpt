--TEST--
ReflectionClass::__toString() - various parameters
--FILE--
<?php
Class A {
	public function privf(Exception $a) {}
	public function pubf(A $a,
						 $b,
						 C $c = null,
						 $d = K,
						 $e = "15 chars long -",
						 $f = null,
						 $g = false,
						 array $h = null) {}
}

Class C extends A { }

define('K', "16 chars long --");
echo new ReflectionClass("C"), "\n";
?>
--EXPECTF--
Class [ <user> class C extends A ] {
  @@ %s 14-14

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [2] {
    Method [ <user, inherits A> public method privf ] {
      @@ %s 3 - 3

      - Parameters [1] {
        Parameter #0 [ <required> Exception $a ]
      }
    }

    Method [ <user, inherits A> public method pubf ] {
      @@ %s 4 - 11

      - Parameters [8] {
        Parameter #0 [ <required> A $a ]
        Parameter #1 [ <required> $b ]
        Parameter #2 [ <optional> C or NULL $c = NULL ]
        Parameter #3 [ <optional> $d = '16 chars long -...' ]
        Parameter #4 [ <optional> $e = '15 chars long -' ]
        Parameter #5 [ <optional> $f = NULL ]
        Parameter #6 [ <optional> $g = false ]
        Parameter #7 [ <optional> array or NULL $h = NULL ]
      }
    }
  }
}
