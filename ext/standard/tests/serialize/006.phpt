--TEST--
serialize()/unserialize() with exotic letters
--FILE--
<?php
    $������ = array('������' => '������');

    class �berK��li��
    {
        public $��������ber = '������';
    }

    $foo = new �berk��li��();

    var_dump(serialize($foo));
    var_dump(unserialize(serialize($foo)));
    var_dump(serialize($������));
    var_dump(unserialize(serialize($������)));
?>
--EXPECT--
string(55) "O:11:"�berK��li��":1:{s:11:"��������ber";s:6:"������";}"
object(�berK��li��)#2 (1) {
  ["��������ber"]=>
  string(6) "������"
}
string(32) "a:1:{s:6:"������";s:6:"������";}"
array(1) {
  ["������"]=>
  string(6) "������"
}
