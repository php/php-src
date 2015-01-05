--TEST--
ZE2 Constructor precedence
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
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

class Base_php7 {
  function __construct() {
    var_dump('Base constructor');
  }
  }

class Child_php7 extends Base_php7 {
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

class Child_mx2 extends Base_php7 {
  function Child_mx2() {
    var_dump('Child constructor');
    parent::__construct();
  }
}

echo "### PHP 4 style\n";
$c4= new Child_php4();

echo "### PHP 7 style\n";
$c5= new Child_php7();

echo "### Mixed style 1\n";
$cm= new Child_mx1();

echo "### Mixed style 2\n";
$cm= new Child_mx2();
?>
--EXPECT--
### PHP 4 style
string(17) "Child constructor"
string(16) "Base constructor"
### PHP 7 style
string(17) "Child constructor"
string(16) "Base constructor"
### Mixed style 1
string(17) "Child constructor"
string(16) "Base constructor"
### Mixed style 2
string(17) "Child constructor"
string(16) "Base constructor"
