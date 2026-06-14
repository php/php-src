--TEST--
Bug #69180: Backtrace does not honor trait conflict resolution / method aliasing
--FILE--
<?php

trait T {
    public function __get($name): string {
        var_dump(__METHOD__);
        print_r(array_map(fn ($v) => $v['class'] . '::' . $v['function'], debug_backtrace()));
        
        return '=' . $name;
    }
}

class Model {
    use T {
        T::__get as private __t_get;
    }
    
    public function __get($name): string {
        var_dump(__METHOD__);
        return $this->__t_get($name);
    }
}

class X extends Model {}

class Y extends Model {
    public function __get($name): string {
        var_dump(__METHOD__);
        return parent::__get($name);
    }
}

class Z extends Model {
    private function __x_get($name): string {
        var_dump(__METHOD__);
        return parent::__get($name);
    }
    
    public function __get($name): string {
        var_dump(__METHOD__);
        return $this->__x_get($name);
    }
}

class P extends Model {
    private function __t_get($name): string {
        var_dump(__METHOD__);
        return parent::__get($name);
    }
    
    public function __get($name): string {
        var_dump(__METHOD__);
        return $this->__t_get($name);
    }
}

$m = new X();
$m->a;

echo "\n";

$m = new Y();
$m->a;

echo "\n";

$m = new Z();
$m->a;

echo "\n";

$m = new P();
$m->a;

?>
--EXPECT--
string(12) "Model::__get"
string(8) "T::__get"
Array
(
    [0] => Model::__t_get
    [1] => Model::__get
)

string(8) "Y::__get"
string(12) "Model::__get"
string(8) "T::__get"
Array
(
    [0] => Model::__t_get
    [1] => Model::__get
    [2] => Y::__get
)

string(8) "Z::__get"
string(10) "Z::__x_get"
string(12) "Model::__get"
string(8) "T::__get"
Array
(
    [0] => Model::__t_get
    [1] => Model::__get
    [2] => Z::__x_get
    [3] => Z::__get
)

string(8) "P::__get"
string(10) "P::__t_get"
string(12) "Model::__get"
string(8) "T::__get"
Array
(
    [0] => Model::__t_get
    [1] => Model::__get
    [2] => P::__t_get
    [3] => P::__get
)
