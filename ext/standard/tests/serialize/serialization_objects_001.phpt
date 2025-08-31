--TEST--
Test serialize() & unserialize() functions: objects
--INI--
serialize_precision=100
--FILE--
<?php
echo "\n--- Testing objects ---\n";

class members
{
  private $var_private = 10;
  protected $var_protected = "string";
  public $var_public = array(-100.123, "string", TRUE);
}

$members_obj = new members();
var_dump( $members_obj );
$serialize_data = serialize( $members_obj );
var_dump( $serialize_data );
$members_obj = unserialize( $serialize_data );
var_dump( $members_obj );

echo "\n--- testing reference to an obj ---\n";
$ref_members_obj = &$members_obj;
$serialize_data = serialize( $ref_members_obj );
var_dump( $serialize_data );
$ref_members_obj = unserialize( $serialize_data );
var_dump( $ref_members_obj );

echo "\nDone";
?>
--EXPECTF--
--- Testing objects ---
object(members)#%d (3) {
  ["var_private":"members":private]=>
  int(10)
  ["var_protected":protected]=>
  string(6) "string"
  ["var_public"]=>
  array(3) {
    [0]=>
    float(-100.1230000000000046611603465862572193145751953125)
    [1]=>
    string(6) "string"
    [2]=>
    bool(true)
  }
}
string(195) "O:7:"members":3:{s:20:"%0members%0var_private";i:10;s:16:"%0*%0var_protected";s:6:"string";s:10:"var_public";a:3:{i:0;d:-100.1230000000000046611603465862572193145751953125;i:1;s:6:"string";i:2;b:1;}}"
object(members)#%d (3) {
  ["var_private":"members":private]=>
  int(10)
  ["var_protected":protected]=>
  string(6) "string"
  ["var_public"]=>
  array(3) {
    [0]=>
    float(-100.1230000000000046611603465862572193145751953125)
    [1]=>
    string(6) "string"
    [2]=>
    bool(true)
  }
}

--- testing reference to an obj ---
string(195) "O:7:"members":3:{s:20:"%0members%0var_private";i:10;s:16:"%0*%0var_protected";s:6:"string";s:10:"var_public";a:3:{i:0;d:-100.1230000000000046611603465862572193145751953125;i:1;s:6:"string";i:2;b:1;}}"
object(members)#%d (3) {
  ["var_private":"members":private]=>
  int(10)
  ["var_protected":protected]=>
  string(6) "string"
  ["var_public"]=>
  array(3) {
    [0]=>
    float(-100.1230000000000046611603465862572193145751953125)
    [1]=>
    string(6) "string"
    [2]=>
    bool(true)
  }
}

Done
