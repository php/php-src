--TEST--
Bug #38287 (static variables mess up global vars)
--FILE--
<?php
error_reporting(0);

something::do_something();

// $not_there is really NULL
var_dump($not_there);

// error occurs here: execution should never get inside the if condition because $not_there is NULL
if ($not_there["invalid_var"]) {
  // will print NULL (which is ok, but execution should never get here if the value is NULL)
  var_dump($not_there["use_authmodule"]);
  // will print "PATH:Array"
  print "PATH:".$not_there["use_authmodule"]."\n";
}

class something {
  public static function get_object() {
    static $object=NULL;
    if ($object===NULL)
    $object=new something;
    return $object;
  }

  public static function do_something() {
    self::get_object()->vars[]=1;
    self::get_object()->vars[]=2;
    self::get_object()->vars[]=3;
    var_dump(self::get_object()->vars);
  }
}
?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
NULL
