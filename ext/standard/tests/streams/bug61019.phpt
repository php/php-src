--TEST--
Bug #61019 (Out of memory on command stream_get_contents)
--FILE--
<?php

echo "Test\n";

$descriptorspec = array(
    0 => array("pipe", "r"),  // stdin is a pipe that the child will read from
    1 => array("pipe", "w"),  // stdout is a pipe that the child will write to
    2 => array("pipe", "w")   // stderr is a pipe that the child will write to
);

$process=proc_open("echo testtext",$descriptorspec,$pipes);
if(is_resource($process))
{
    stream_set_blocking($pipes[0],false);
    stream_set_blocking($pipes[1],false);
    stream_set_blocking($pipes[2],false);
    stream_set_write_buffer($pipes[0],0);
    stream_set_read_buffer($pipes[1],0);
    stream_set_read_buffer($pipes[2],0);
    $stdin_stream="";
    $stderr_stream="";

    echo "External command executed\n";
    do
    {
        $process_state=proc_get_status($process);
        $tmp_stdin=stream_get_contents($pipes[1]);
        if($tmp_stdin)
        {
            $stdin_stream=$stdin_stream.$tmp_stdin;
        }
        $tmp_stderr=stream_get_contents($pipes[2]);
        if($tmp_stderr)
        {
            $stderr_stream=$stderr_stream.$tmp_stderr;
        }
    } while($process_state['running']);

    echo "External command exit: ".$process_state['exitcode']."\n";

    //read outstanding data
    $tmp_stdin=stream_get_contents($pipes[1]);
    if($tmp_stdin)
    {
        $stdin_stream=$stdin_stream.$tmp_stdin;
    }
    $tmp_stderr=stream_get_contents($pipes[2]);
    if($tmp_stderr)
    {
        $stderr_stream=$stderr_stream.$tmp_stderr;
    }

    fclose ($pipes[0]);
    fclose ($pipes[1]);
    fclose ($pipes[2]);

    proc_close($process);

    echo "STDOUT: ".$stdin_stream."\n";
    echo "STDERR: ".$stderr_stream."\n";
}
else
{
    echo "Can't start external command\n";
}
?>
--EXPECT--
Test
External command executed
External command exit: 0
STDOUT: testtext

STDERR: 
