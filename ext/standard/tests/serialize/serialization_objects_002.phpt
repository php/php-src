--TEST--
Test serialize() & unserialize() functions: objects (variations)
--INI--
serialize_precision=100
--FILE--
<?php
echo "\n--- Testing Variations in objects ---\n";

class members
{
  private $var_private = 10;
  protected $var_protected = "string";
  public $var_public = array(-100.123, "string", TRUE);
}

class nomembers { }

class C {
  var $a, $b, $c, $d, $e, $f, $g, $h;
  function __construct() {
    $this->a = 10;
    $this->b = "string";
    $this->c = TRUE;
    $this->d = -2.34444;
    $this->e = array(1, 2.22, "string", TRUE, array(),
                     new members(), null);
    $this->f = new nomembers();
    $this->g = NULL;
    $this->h = NULL;
  }
}

class D extends C {
  function __construct( $w, $x, $y, $z ) {
    $this->a = $w;
    $this->b = $x;
    $this->c = $y;
    $this->d = $z;
  }
}

$variation_obj_arr = array(
  new C(),
  new D( 1, 2, 3333, 444444 ),
  new D( .5, 0.005, -1.345, 10.005e5 ),
  new D( TRUE, true, FALSE, false ),
  new D( "a", 'a', "string", 'string' ),
  new D( array(),
         array(1, 2.222, TRUE, FALSE, "string"),
         array(new nomembers(), $file_handle, NULL, ""),
         array(array(1,2,3,array()))
       ),
  new D( NULL, null, "", "\0" ),
  new D( new members, new nomembers, $file_handle, NULL),
);

/* Testing serialization on all the objects through loop */
foreach( $variation_obj_arr as $object) {

  echo "After Serialization => ";
  $serialize_data = serialize( $object );
  var_dump( $serialize_data );

  echo "After Unserialization => ";
  $unserialize_data = unserialize( $serialize_data );
  var_dump( $unserialize_data );
}

echo "\nDone";
?>
--EXPECTF--
--- Testing Variations in objects ---

Warning: Undefined variable $file_handle in %s on line %d

Warning: Undefined variable $file_handle in %s on line %d
After Serialization => string(493) "O:1:"C":8:{s:1:"a";i:10;s:1:"b";s:6:"string";s:1:"c";b:1;s:1:"d";d:-2.344440000000000079438677857979200780391693115234375;s:1:"e";a:7:{i:0;i:1;i:1;d:2.220000000000000195399252334027551114559173583984375;i:2;s:6:"string";i:3;b:1;i:4;a:0:{}i:5;O:7:"members":3:{s:20:"%0members%0var_private";i:10;s:16:"%0*%0var_protected";s:6:"string";s:10:"var_public";a:3:{i:0;d:-100.1230000000000046611603465862572193145751953125;i:1;s:6:"string";i:2;b:1;}}i:6;N;}s:1:"f";O:9:"nomembers":0:{}s:1:"g";N;s:1:"h";N;}"
After Unserialization => object(C)#%d (8) {
  ["a"]=>
  int(10)
  ["b"]=>
  string(6) "string"
  ["c"]=>
  bool(true)
  ["d"]=>
  float(-2.344440000000000079438677857979200780391693115234375)
  ["e"]=>
  array(7) {
    [0]=>
    int(1)
    [1]=>
    float(2.220000000000000195399252334027551114559173583984375)
    [2]=>
    string(6) "string"
    [3]=>
    bool(true)
    [4]=>
    array(0) {
    }
    [5]=>
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
    [6]=>
    NULL
  }
  ["f"]=>
  object(nomembers)#%d (0) {
  }
  ["g"]=>
  NULL
  ["h"]=>
  NULL
}
After Serialization => string(108) "O:1:"D":8:{s:1:"a";i:1;s:1:"b";i:2;s:1:"c";i:3333;s:1:"d";i:444444;s:1:"e";N;s:1:"f";N;s:1:"g";N;s:1:"h";N;}"
After Unserialization => object(D)#%d (8) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(3333)
  ["d"]=>
  int(444444)
  ["e"]=>
  NULL
  ["f"]=>
  NULL
  ["g"]=>
  NULL
  ["h"]=>
  NULL
}
After Serialization => string(223) "O:1:"D":8:{s:1:"a";d:0.5;s:1:"b";d:0.005000000000000000104083408558608425664715468883514404296875;s:1:"c";d:-1.3449999999999999733546474089962430298328399658203125;s:1:"d";d:1000500;s:1:"e";N;s:1:"f";N;s:1:"g";N;s:1:"h";N;}"
After Unserialization => object(D)#%d (8) {
  ["a"]=>
  float(0.5)
  ["b"]=>
  float(0.005000000000000000104083408558608425664715468883514404296875)
  ["c"]=>
  float(-1.3449999999999999733546474089962430298328399658203125)
  ["d"]=>
  float(1000500)
  ["e"]=>
  NULL
  ["f"]=>
  NULL
  ["g"]=>
  NULL
  ["h"]=>
  NULL
}
After Serialization => string(100) "O:1:"D":8:{s:1:"a";b:1;s:1:"b";b:1;s:1:"c";b:0;s:1:"d";b:0;s:1:"e";N;s:1:"f";N;s:1:"g";N;s:1:"h";N;}"
After Unserialization => object(D)#%d (8) {
  ["a"]=>
  bool(true)
  ["b"]=>
  bool(true)
  ["c"]=>
  bool(false)
  ["d"]=>
  bool(false)
  ["e"]=>
  NULL
  ["f"]=>
  NULL
  ["g"]=>
  NULL
  ["h"]=>
  NULL
}
After Serialization => string(126) "O:1:"D":8:{s:1:"a";s:1:"a";s:1:"b";s:1:"a";s:1:"c";s:6:"string";s:1:"d";s:6:"string";s:1:"e";N;s:1:"f";N;s:1:"g";N;s:1:"h";N;}"
After Unserialization => object(D)#%d (8) {
  ["a"]=>
  string(1) "a"
  ["b"]=>
  string(1) "a"
  ["c"]=>
  string(6) "string"
  ["d"]=>
  string(6) "string"
  ["e"]=>
  NULL
  ["f"]=>
  NULL
  ["g"]=>
  NULL
  ["h"]=>
  NULL
}
After Serialization => string(300) "O:1:"D":8:{s:1:"a";a:0:{}s:1:"b";a:5:{i:0;i:1;i:1;d:2.221999999999999975131004248396493494510650634765625;i:2;b:1;i:3;b:0;i:4;s:6:"string";}s:1:"c";a:4:{i:0;O:9:"nomembers":0:{}i:1;N;i:2;N;i:3;s:0:"";}s:1:"d";a:1:{i:0;a:4:{i:0;i:1;i:1;i:2;i:2;i:3;i:3;a:0:{}}}s:1:"e";N;s:1:"f";N;s:1:"g";N;s:1:"h";N;}"
After Unserialization => object(D)#%d (8) {
  ["a"]=>
  array(0) {
  }
  ["b"]=>
  array(5) {
    [0]=>
    int(1)
    [1]=>
    float(2.221999999999999975131004248396493494510650634765625)
    [2]=>
    bool(true)
    [3]=>
    bool(false)
    [4]=>
    string(6) "string"
  }
  ["c"]=>
  array(4) {
    [0]=>
    object(nomembers)#%d (0) {
    }
    [1]=>
    NULL
    [2]=>
    NULL
    [3]=>
    string(0) ""
  }
  ["d"]=>
  array(1) {
    [0]=>
    array(4) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
      [3]=>
      array(0) {
      }
    }
  }
  ["e"]=>
  NULL
  ["f"]=>
  NULL
  ["g"]=>
  NULL
  ["h"]=>
  NULL
}
After Serialization => string(103) "O:1:"D":8:{s:1:"a";N;s:1:"b";N;s:1:"c";s:0:"";s:1:"d";s:1:"%0";s:1:"e";N;s:1:"f";N;s:1:"g";N;s:1:"h";N;}"
After Unserialization => object(D)#%d (8) {
  ["a"]=>
  NULL
  ["b"]=>
  NULL
  ["c"]=>
  string(0) ""
  ["d"]=>
  string(1) "%0"
  ["e"]=>
  NULL
  ["f"]=>
  NULL
  ["g"]=>
  NULL
  ["h"]=>
  NULL
}
After Serialization => string(303) "O:1:"D":8:{s:1:"a";O:7:"members":3:{s:20:"%0members%0var_private";i:10;s:16:"%0*%0var_protected";s:6:"string";s:10:"var_public";a:3:{i:0;d:-100.1230000000000046611603465862572193145751953125;i:1;s:6:"string";i:2;b:1;}}s:1:"b";O:9:"nomembers":0:{}s:1:"c";N;s:1:"d";N;s:1:"e";N;s:1:"f";N;s:1:"g";N;s:1:"h";N;}"
After Unserialization => object(D)#%d (8) {
  ["a"]=>
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
  ["b"]=>
  object(nomembers)#%d (0) {
  }
  ["c"]=>
  NULL
  ["d"]=>
  NULL
  ["e"]=>
  NULL
  ["f"]=>
  NULL
  ["g"]=>
  NULL
  ["h"]=>
  NULL
}

Done
