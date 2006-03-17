--TEST--
strtr() function (with unicode chars and combining sequences)
--FILE--
<?php
	declare(encoding="utf8");
	$from = "aåаиу";
	$to   = "12356";
	$string = "Dе åkаt krаpt de krуllen van de trap af";
	var_dump( strtr( $string, $from, $to ) );

	$from = "aeoui";
	$to   = "12356";
	$string = "De akat krapt de krullen van de trap af";
	var_dump( strtr( $string, $from, $to ) );

	$ar = array( "a" => 1, "e" => "2", "o" => 3, "u" => 5, "i" => 6 );
	$string = "De akat krapt de krullen van de trap af";
	var_dump( strtr( $string, $ar ) );

	// Test with combining chars
	$from = "åb";
	$to   = "1";
	$string = "xyzabc";
	var_dump( strtr( $string, $from, $to ) );
?>
--EXPECT--
unicode(39) "Dе 2k3t kr3pt de kr6llen v1n de tr1p 1f"
unicode(39) "D2 1k1t kr1pt d2 kr5ll2n v1n d2 tr1p 1f"
unicode(39) "D2 1k1t kr1pt d2 kr5ll2n v1n d2 tr1p 1f"
