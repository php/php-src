--TEST--
Constructor precedence
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
  namespace php4 {
    class Base {
      function Base() {
        var_dump('Base constructor');
      }
    }

    class Child extends Base {
      function Child() {
        var_dump('Child constructor');
        parent::Base();
      }
    }
  }
  
  namespace php5 {
    class Base {
      function __construct() {
        var_dump('Base constructor');
      }
      
      function Base() {
        var_dump('I should not be called');
      }
    }

    class Child extends Base {
      function __construct() {
        var_dump('Child constructor');
        parent::__construct();
      }
      
      function Child() {
        var_dump('I should not be called');
      }
    }
  }

  namespace mixed1 {
    class Child extends php4::Base {
      function __construct() {
        var_dump('Child constructor');
        parent::Base();
      }
    }
  }
  
  namespace mixed2 {
    class Child extends php5::Base {
      function Child() {
        var_dump('Child constructor');
        parent::__construct();
      }
    }
  }
  
  echo "### PHP4 style\n";
  $c4= new php4::Child();

  echo "### PHP5 style\n";
  $c5= new php5::Child();
  
  echo "### Mixed style 1\n";
  $cm= new mixed1::Child();

  echo "### Mixed style 2\n";
  $cm= new mixed2::Child();
?>
--EXPECT--
### PHP4 style
string(17) "Child constructor"
string(16) "Base constructor"
### PHP5 style
string(17) "Child constructor"
string(16) "Base constructor"
### Mixed style 1
string(17) "Child constructor"
string(16) "Base constructor"
### Mixed style 2
string(17) "Child constructor"
string(16) "Base constructor"