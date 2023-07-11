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
exec('ps -p 1 -o "pid,nice"', $output, $exit_code);
if ($exit_code !== 0) {
    die("skip ps -p is not available");
}
?>
--FILE--
<?php
    function getNice($id)
    {
        $res = shell_exec('ps -p ' . $id .' -o "pid,nice"');
        preg_match('/^\s*\w+\s+\w+\s*(\d+)\s+(-?\d+)/m', $res, $matches);
        if (count($matches) > 2)
            return $matches[2];
        else
            return -1;
    }
    $delta = 5;
    $pid = getmypid();
    $niceBefore = getNice($pid);
    proc_nice($delta);
    $niceAfter = getNice($pid);
    // The maximum niceness level is 19, if the process is already running at a high niceness, it cannot be increased.
    // Decreasing is only possible for superusers.
    var_dump(min($niceBefore + $delta, 19) == $niceAfter);
?>
--EXPECT--
bool(true)
