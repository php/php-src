--TEST--
serialize()/unserialize() with exotic letters
--FILE--
<?php
    $åäöÅÄÖ = array('åäöÅÄÖ' => 'åäöÅÄÖ');

    class ÜberKööliäå
    {
        public $åäöÅÄÖüÜber = 'åäöÅÄÖ';
    }

    $foo = new ÜberKööliäå();

    var_dump(serialize($foo));
    var_dump(unserialize(serialize($foo)));
    var_dump(serialize($åäöÅÄÖ));
    var_dump(unserialize(serialize($åäöÅÄÖ)));
?>
--EXPECT--
string(55) "O:11:"ÜberKööliäå":1:{s:11:"åäöÅÄÖüÜber";s:6:"åäöÅÄÖ";}"
object(ÜberKööliäå)#2 (1) {
  ["åäöÅÄÖüÜber"]=>
  string(6) "åäöÅÄÖ"
}
string(32) "a:1:{s:6:"åäöÅÄÖ";s:6:"åäöÅÄÖ";}"
array(1) {
  ["åäöÅÄÖ"]=>
  string(6) "åäöÅÄÖ"
}
