--TEST--
GH-22681: null bytes in name truncate ReflectionFunction::__toString()
--FILE--
<?php

$obj = new class {
    public function make(): Closure {
        return function () {};
    }
};

$r = new ReflectionFunction($obj->make());
echo $r;
var_dump( $r->getName() );

?>
--EXPECTF--
Closure [ <user> public method {closure:class@anonymous%0%s::make():%d} ] {
  @@ %s %d - %d
}
string(%d) "{closure:class@anonymous%0%s::make():%d}"
