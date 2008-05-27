--TEST--
serialize()/unserialize() with exotic letters
--INI--
unicode.script_encoding=ISO-8859-1
unicode.output_encoding=ISO-8859-1
--FILE--
<?php
	$åäöÅÄÖ = array('åäöÅÄÖ' => 'åäöÅÄÖ');

	class ÜberKööliäå 
	{
		public $åäöÅÄÖüÜber = 'åäöÅÄÖ';
	}
  
    $foo = new Überkööliäå();
  
	var_dump(serialize($foo));
	var_dump(unserialize(serialize($foo)));
	var_dump(serialize($åäöÅÄÖ));
	var_dump(unserialize(serialize($åäöÅÄÖ)));
?>
--EXPECT--
unicode(131) "O:11:"\00dcberK\00f6\00f6li\00e4\00e5":1:{U:11:"\00e5\00e4\00f6\00c5\00c4\00d6\00fc\00dcber";U:6:"\00e5\00e4\00f6\00c5\00c4\00d6";}"
object(ÜberKööliäå)#2 (1) {
  [u"åäöÅÄÖüÜber"]=>
  unicode(6) "åäöÅÄÖ"
}
unicode(80) "a:1:{U:6:"\00e5\00e4\00f6\00c5\00c4\00d6";U:6:"\00e5\00e4\00f6\00c5\00c4\00d6";}"
array(1) {
  [u"åäöÅÄÖ"]=>
  unicode(6) "åäöÅÄÖ"
}
