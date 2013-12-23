--TEST--
--FILE--
<?php
class test implements Serializable {
    public $a;

    public function __construct( $_val ){
        $this->a = $_val;
    }

    public function serialize() {
        $tmp = (object) array();
        $tmp->a = $this->a;
        echo serialize( $tmp ) . "\n";
        return serialize( $tmp );
    }

    public function unserialize($var) {}
}

$n = array();
$n[] = new test("temp1");
$n[] = new test("temp2");
$n[] = new test("temp3");
$n[] = new test("temp4");
$n[] = new test("temp5");
$n[] = new test("temp6");
$n[] = new test("temp7");
$n[] = new test("temp8");
$n[] = new test("temp9");
print_r( unserialize( serialize( $n ) ) );
?>
--EXPECTF--
O:8:"stdClass":1:{s:1:"a";s:5:"temp1";}
O:8:"stdClass":1:{s:1:"a";s:5:"temp2";}
O:8:"stdClass":1:{s:1:"a";s:5:"temp3";}
O:8:"stdClass":1:{s:1:"a";s:5:"temp4";}
O:8:"stdClass":1:{s:1:"a";s:5:"temp5";}
O:8:"stdClass":1:{s:1:"a";s:5:"temp6";}
O:8:"stdClass":1:{s:1:"a";s:5:"temp7";}
O:8:"stdClass":1:{s:1:"a";s:5:"temp8";}
O:8:"stdClass":1:{s:1:"a";s:5:"temp9";}
Array
(
    [0] => test Object
        (
            [a] => 
        )

    [1] => test Object
        (
            [a] => 
        )

    [2] => test Object
        (
            [a] => 
        )

    [3] => test Object
        (
            [a] => 
        )

    [4] => test Object
        (
            [a] => 
        )

    [5] => test Object
        (
            [a] => 
        )

    [6] => test Object
        (
            [a] => 
        )

    [7] => test Object
        (
            [a] => 
        )

    [8] => test Object
        (
            [a] => 
        )

)

