--TEST--
fribidi_log2vis
--SKIPIF--
<?php if (!extension_loaded("fribidi")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
	error_reporting (E_ALL ^ E_NOTICE);

	/* According to Tal, this is not really even used anywhere.. */
	// $a =  fribidi_log2vis("THE dog 123 IS THE biggest", FRIBIDI_AUTO, FRIBIDI_CHARSET_CAP_RTL);

	$b =  fribidi_log2vis("בלה 198 foo בלה BAR 12", FRIBIDI_RTL, FRIBIDI_CHARSET_8859_8);
	$c =  fribidi_log2vis("Fri בידי יאאא bla 12% bla", FRIBIDI_AUTO, FRIBIDI_CHARSET_CP1255);

	$d = array($b, $c);
	var_dump($d);
?>
--EXPECT--
array(2) {
  [0]=>
  string(22) "BAR 12 הלב foo 198 הלב"
  [1]=>
  string(25) "Fri אאאי ידיב bla 12% bla"
}
