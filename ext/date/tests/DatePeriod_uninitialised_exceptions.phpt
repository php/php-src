--TEST--
DateTime uninitialised exceptions
--INI--
date.timezone=Europe/London
--FILE--
<?php
class MyDatePeriod extends DatePeriod
{
	function __construct()
	{
	}
}

function check(callable $c)
{
	try {
		var_dump($c());
	} catch (\DateObjectError $e) {
		echo $e::class, ': ', $e->getMessage(), "\n";
	}
}

$mdp = new MyDatePeriod();

check(fn() => serialize($mdp));
check(fn() => $mdp->getStartDate());
check(fn() => $mdp->getDateInterval());

check(function() use ($mdp) {
	foreach($mdp as $foo)
	{
	}
});

/* Allowed to be empty */
check(fn() => $mdp->getEndDate());
check(fn() => $mdp->getRecurrences());
?>
--EXPECTF--
DateObjectError: Object of type MyDatePeriod (inheriting DatePeriod) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDatePeriod (inheriting DatePeriod) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDatePeriod (inheriting DatePeriod) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type DatePeriod has not been correctly initialized by calling parent::__construct() in its constructor
NULL
NULL
