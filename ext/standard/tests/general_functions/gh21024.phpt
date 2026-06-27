--TEST--
GH-21024 (UAF in var_dump()/debug_zval_dump()/var_export() when a callback frees the dumped object)
--FILE--
<?php
class Bar {
    public function __debugInfo(): array {
        echo $undefined;
        return ['ok' => 1];
    }
}
class Hooked {
    public $x { get { echo $undefined; return 1; } }
}

$a = [new Bar];
$ra = &$a[0];
set_error_handler(function () use (&$ra) { $ra = null; });
var_dump($a);
restore_error_handler();

$b = [new Bar];
$rb = &$b[0];
set_error_handler(function () use (&$rb) { $rb = null; });
debug_zval_dump($b);
restore_error_handler();

$c = [new Hooked];
$rc = &$c[0];
set_error_handler(function () use (&$rc) { $rc = null; });
var_export($c);
echo "\n";
restore_error_handler();

echo "survived\n";
?>
--EXPECTF--
array(1) {
  [0]=>
  &object(Bar)#%d (1) {
    ["ok"]=>
    int(1)
  }
}
array(1) packed refcount(%d){
  [0]=>
  reference refcount(%d) {
    object(Bar)#%d (1) refcount(%d){
      ["ok"]=>
      int(1)
    }
  }
}
array (
  0 => 
  \Hooked::__set_state(array(
     'x' => 1,
  )),
)
survived
