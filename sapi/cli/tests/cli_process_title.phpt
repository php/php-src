--TEST--
Check cli_process_title support
--SKIPIF--
<?php
    if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') print "skip";
?>
--FILE--
<?php
echo "*** Testing setting the process title ***\n";

$set_title = $original_title = "test title for php cli";
$pid = getmypid();

if (cli_set_process_title($original_title) === true)
  echo "Successfully set title\n";

$ps_output = shell_exec("ps -p $pid -o command | tail -n 1");
if ($ps_output === null) {
  echo "ps failed\n";
  die();
}

$load_title = trim($ps_output);
if (strpos(strtoupper(substr(PHP_OS, 0, 13)), "BSD") !== false)
{
  // Fix title for BSD
  $set_title = "php: $original_title (php)";
}

if ($load_title == $set_title)
  echo "Successfully verified title using ps\n";
else
  echo "Actually loaded from ps: $load_title\n";

$read_title = cli_get_process_title();
if ($read_title == $original_title)
  echo "Successfully verified title using get\n";
else
  echo "Actually loaded from get: $read_title\n";

?>
--EXPECTF--
*** Testing setting the process title ***
Successfully set title
Successfully verified title using ps
Successfully verified title using get
