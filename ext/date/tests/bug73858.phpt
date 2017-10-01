--TEST--
Bug #73858: diff() of two relative/described DateTimes is wrong
--FILE--
<?php
/*
In the "verbose setup method" I'm trying setup the DateTime object myself
to see if it's the format string which is parsed in correctly or if it's the DateTime
object which is breaking stuff. From the testing it appears DateTime is broken somehow.
*/
$ss = 'february first day of last month midnight';
$es = 'february first day of this month midnight - 1 second';

$s = new DateTime($ss);
$e = new DateTime($es);
$d= $e->diff($s);
var_dump($d->days); // 0 ... but should be 30

$s = (new DateTime(null))->setTimestamp(strtotime($ss)); // verbose setup method
$e = (new DateTime(null))->setTimestamp(strtotime($es)); // verbose setup method
$d = $e->diff($s);
var_dump($d->days); // 30 ... and should be 30

/*
Next we will try mix/match the code to see what happens, surprisingly it seems that the end date ($e)
is the important one, if it uses the verbose method it returns the correct values.
*/
$s = (new DateTime(null))->setTimestamp(strtotime($ss)); // verbose setup method
$e = new DateTime($es);
$d= $e->diff($s);
var_dump($d->days); // 0 ... but should be 30

$s = new DateTime($ss);
$e = (new DateTime(null))->setTimestamp(strtotime($es)); // verbose setup method
$d= $e->diff($s);
var_dump($d->days); // 30 ... and should be 30

/*
This test just proves that the $e date is important BUT NOT because it's the one we call the diff() method
on, that's just coincidental that seems to imply that the "- 1 second" in the date string is the problem.
*/
$s = new DateTime($ss);
$e = (new DateTime(null))->setTimestamp(strtotime($es)); // verbose setup method
$d= $s->diff($e);
var_dump($d->days); // 30 ... and should be 30

/*
[Workaround]
This final test seems to prove that the input string is important and that the "- 1 secord" has a negative knock-on
effect on the results of the diff. By modifying the datetime with ->modify everything works as expected ...
it just means you have to be careful of how we work with DateTimes .
*/
$s = new DateTime($ss);
$e = new DateTime('february first day of this month midnight');
$e->modify('- 1 second');
var_dump($e->diff($s)->days); // 30 ... and should be 30
?>
--EXPECT--
int(30)
int(30)
int(30)
int(30)
int(30)
int(30)
