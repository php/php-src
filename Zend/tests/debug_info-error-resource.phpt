--TEST--
Testing __debugInfo() magic method with bad returns RESOURCE
--INI--
allow_url_fopen=1
--FILE--
<?php

class C {
  public $val;
  public function __debugInfo() {
    return $this->val;
  }
  public function __construct($val) {
    $this->val = $val;
  }
}

$c = new C(fopen("data:text/plain,Foo", 'r'));
var_dump($c);
?>
--EXPECTF--
Fatal error: Uncaught Error: __debuginfo() must return an array in %s%eZend%etests%edebug_info-error-resource.php:14
Stack trace:
#0 %s(14): var_dump(Object(C))
#1 {main}
  thrown in %s on line 14
