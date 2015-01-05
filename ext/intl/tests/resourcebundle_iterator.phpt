--TEST--
Test ResourceBundle iterator
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
	include "resourcebundle.inc";

	// fall back
	$r = new ResourceBundle( 'en_US', BUNDLE );

	foreach ($r as $onekey => $oneval) {
		echo "Here comes $onekey:\n";
		switch (gettype($oneval)) {
		  case 'string':
		    echo bin2hex( $oneval ) . "\n";
		    break;

		  case 'integer':
		    echo "$oneval\n";
		    break;

		  default:
		    print_r( $oneval );
		}
		echo "\n";
	}

	echo "Testarray Contents:\n";
	$r = $r->get( 'testarray' );
	foreach ($r as $onekey => $oneval) {
	   echo "$onekey => $oneval\n";
	}
?>
--EXPECTF--
Here comes testarray:
ResourceBundle Object
(
)

Here comes testbin:
a1b2c3d4e5f67890

Here comes testint:
2

Here comes teststring:
48656c6c6f20576f726c6421

Here comes testtable:
ResourceBundle Object
(
)

Here comes testvector:
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 4
    [4] => 5
    [5] => 6
    [6] => 7
    [7] => 8
    [8] => 9
    [9] => 0
)

Testarray Contents:
0 => string 1
1 => string 2
2 => string 3
