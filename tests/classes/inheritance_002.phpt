--TEST--
ZE2 Constructor precedence
--FILE--
<?php
class Base_php4 {
  function Base_php4() {
    var_dump('Base constructor');
  }
}

class Child_php4 extends Base_php4 {
  function Child_php4() {
    var_dump('Child constructor');
    parent::Base_php4();
  }
}

class Base_php5 {
  function __construct() {
    var_dump('Base constructor');
  }
  }

class Child_php5 extends Base_php5 {
  function __construct() {
    var_dump('Child constructor');
    parent::__construct();
  }
  }

class Child_mx1 extends Base_php4 {
  function __construct() {
    var_dump('Child constructor');
    parent::Base_php4();
  }
}

class Child_mx2 extends Base_php5 {
  function Child_mx2() {
    var_dump('Child constructor');
    parent::__construct();
  }
}

echo "### PHP 4 style\n";
$c4= new Child_php4();

echo "### PHP 5 style\n";
$c5= new Child_php5();

echo "### Mixed style 1\n";
$cm= new Child_mx1();

echo "### Mixed style 2\n";
$cm= new Child_mx2();
?>
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; Base_php4 has a deprecated constructor in %s on line %d

Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; Child_php4 has a deprecated constructor in %s on line %d

Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; Child_mx2 has a deprecated constructor in %s on line %d
### PHP 4 style
string(17) "Child constructor"
string(16) "Base constructor"
### PHP 5 style
string(17) "Child constructor"
string(16) "Base constructor"
### Mixed style 1
string(17) "Child constructor"
string(16) "Base constructor"
### Mixed style 2
string(17) "Child constructor"
string(16) "Base constructor"
