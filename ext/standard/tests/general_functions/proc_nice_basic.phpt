--TEST--
proc_nice() basic behaviour
--CREDITS--
Italian PHP TestFest 2009 Cesena 19-20-21 june
Fabio Fabbrucci (fabbrucci@grupporetina.com)
Michele Orselli (mo@ideato.it)
Simone Gentili (sensorario@gmail.com)
--SKIPIF--
<?php
if(!function_exists('proc_nice')) die("skip. proc_nice not available ");
if(substr(strtoupper(PHP_OS), 0, 3) == 'WIN') die('skip. not for Windows');
?>
--FILE--
<?php
	function getNice($id)
	{
		$res = shell_exec('ps -p ' . $id .' -o "pid,nice"');
		preg_match('/^\s*\w+\s+\w+\s*(\d+)\s+(\d+)/m', $res, $matches);
		if (count($matches) > 2)
			return $matches[2];
		else
			return -1;
	}
	$delta = 10;
	$pid = getmypid();
	$niceBefore = getNice($pid);
	proc_nice($delta);
	$niceAfter = getNice($pid);
	var_dump($niceBefore == ($niceAfter - $delta));
?>
--EXPECTF--
bool(true)
