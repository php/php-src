--TEST--
IntlGregorianCalendar::__construct(): bad arguments
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(intlgregcal_create_instance(1,2,3,4,5,6,7));
var_dump(intlgregcal_create_instance(1,2,3,4,5,6,7,8));
var_dump(intlgregcal_create_instance(1,2,3,4));
var_dump(new IntlGregorianCalendar(1,2,NULL,4));
var_dump(new IntlGregorianCalendar(1,2,3,4,NULL,array()));


--EXPECTF--

Warning: intlgregcal_create_instance(): intlgregcal_create_instance: too many arguments in %s on line %d
NULL

Warning: intlgregcal_create_instance(): intlgregcal_create_instance: too many arguments in %s on line %d
NULL

Warning: intlgregcal_create_instance(): intlgregcal_create_instance: no variant with 4 arguments (excluding trailing NULLs) in %s on line %d
NULL

Warning: IntlGregorianCalendar::__construct(): intlgregcal_create_instance: no variant with 4 arguments (excluding trailing NULLs) in %s on line %d
NULL

Warning: IntlGregorianCalendar::__construct() expects parameter 6 to be integer, array given in %s on line %d

Warning: IntlGregorianCalendar::__construct(): intlgregcal_create_instance: bad arguments in %s on line %d
NULL
