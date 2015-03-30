--TEST--
Test ResourceBundle::__construct() - existing/missing bundles/locales
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
	
include "resourcebundle.inc";

function ut_main() {
	$str_res = '';
	// all fine
	$r1 = ut_resourcebundle_create( 'root', BUNDLE );
	$str_res .= debug( $r1 );
	$str_res .= print_r( $r1['teststring'], true)."\n";


	// non-root one
	$r1 = ut_resourcebundle_create( 'es', BUNDLE );
	$str_res .= debug( $r1 );
	$str_res .= print_r( $r1['teststring'], true)."\n";

	// fall back
	$r1 = ut_resourcebundle_create( 'en_US', BUNDLE );
        $str_res .= debug( $r1 );
	$str_res .= print_r( $r1['testsring'], true);

	try {
		// fall out
		$r2 = ut_resourcebundle_create( 'en_US', BUNDLE, false );
	}
	catch (\IntlException $ie) {
		$str_res .= "ie: ".$ie->getMessage().PHP_EOL;
	}

	try {
		// missing
		$r3 = ut_resourcebundle_create( 'en_US', 'nonexisting' );
	}
	catch (\IntlException $ie) {
		$str_res .= "ie: ".$ie->getMessage().PHP_EOL;
	}

	return $str_res;
}

	include_once( 'ut_common.inc' );
	ut_run();
?>
--EXPECTF--
ResourceBundle Object
(
)

    0: U_ZERO_ERROR
Hello World!
ResourceBundle Object
(
)

    0: U_ZERO_ERROR
Hola Mundo!
ResourceBundle Object
(
)

 -127: U_USING_DEFAULT_WARNING
ie: resourcebundle_ctor: Cannot load libICU resource bundle
ie: resourcebundle_ctor: Cannot load libICU resource bundle
