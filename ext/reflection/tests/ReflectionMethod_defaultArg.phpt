--TEST--
ReflectionMethod and RECV_INIT (bug #70957 and #70958)
--FILE--
<?php
Abstract class F {
    private function bar($a = self::class) {}
}

Trait T
{
    private function bar($a = self::class) {}
}


class B {
    use T;
}

echo new \ReflectionMethod('F', 'bar');
echo new \ReflectionMethod('T', 'bar');
echo new \ReflectionMethod('B', 'bar');
?>
--EXPECTF--
Method [ <user> private method bar ] {
  @@ %s

  - Parameters [1] {
    Parameter #0 [ <optional> $a = 'F' ]
  }
}
Method [ <user> private method bar ] {
  @@ %s

  - Parameters [1] {
    Parameter #0 [ <optional> $a = self::class ]
  }
}
Method [ <user> private method bar ] {
  @@ %s

  - Parameters [1] {
    Parameter #0 [ <optional> $a = self::class ]
  }
}
