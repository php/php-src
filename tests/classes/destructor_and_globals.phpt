--TEST--
ZE2 accessing globals from destructor in shutdown
--FILE--
<?php
$test_cnt = 0;
$test_num = 0;

function Show() {
  global $test_cnt;
  echo "Count: $test_cnt\n";
}

class counter {
  protected $id;

  public function __construct() {
    global $test_cnt, $test_num;
    $test_cnt++;
    $this->id = $test_num++;
  }

  public function Show() {
    echo 'Id: '.$this->id."\n";
  }

  // try protected here
  public function __destruct() {
    global $test_cnt;
    $test_cnt--;
  }

  static public function destroy(&$obj) {
    $obj = NULL;
    }
}
Show();
$obj1 = new counter;
$obj1->Show();
Show();
$obj2 = new counter;
$obj2->Show();
Show();
counter::destroy($obj1);
Show();
// or uncomment this line and it works
//counter::destroy($obj2);
echo "Done\n";
?>
--EXPECT--
Count: 0
Id: 0
Count: 1
Id: 1
Count: 2
Count: 1
Done
