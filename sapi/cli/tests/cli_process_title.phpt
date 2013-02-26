--TEST--
Check cli_process_title support
--FILE--
<?php
echo "*** Testing setting the process title ***\n";

$set_title = $original_title = uniqid("title", true);
$pid = getmypid();

if (cli_set_process_title($original_title) === true)
  echo "Successfully set title\n";

if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN')
  $ps_output = shell_exec("PowerShell \"get-process cmd*,powershell* | Select-Object mainWindowTitle | ft -hide\"");
else
  $ps_output = shell_exec("ps -p $pid -o command | tail -n 1");

if ($ps_output === null)
{
  echo "ps failed\n";
  die();
}

$load_title = trim($ps_output);
if (strpos(strtoupper(substr(PHP_OS, 0, 13)), "BSD") !== false)
{
  // Fix title for BSD
  $set_title = "php: $original_title (php)";
}
else if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN')
{
  // Kind of convoluted. So when the test is run, the console where the run-tests.php
  // is executed forks a php.exe, which forks a cmd.exe, which then forks a final php.exe
  // to run the actual test. But the console title is set for the original console.
  // I couldn't figure out a good way to navigate this, so we're "grep'ing" all possible
  // console windows for our very unique title. It should occur exactly once in the grep
  // output.
  $pos = strpos($ps_output, $set_title, 0);
  if ($pos !== false)
  {
    $pos = strpos($ps_output, $set_title, $pos + strlen($set_title));
    if ($pos === false)
      $load_title = $set_title;
  }
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
