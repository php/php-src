<?php

function resetVT100State()
{
    $state = array(
        sapi_windows_vt100_support(STDIN),
        sapi_windows_vt100_support(STDOUT),
        sapi_windows_vt100_support(STDERR),
    );
    sapi_windows_vt100_support(STDIN, false);
    sapi_windows_vt100_support(STDOUT, false);
    sapi_windows_vt100_support(STDERR, false);

    return $state;
}

function restoreVT100State(array $state)
{
    sapi_windows_vt100_support(STDIN, $state[0]);
    sapi_windows_vt100_support(STDOUT, $state[1]);
    sapi_windows_vt100_support(STDERR, $state[2]);
}

function testToStdOut()
{
    $state = resetVT100State();

    $sampleStreams = array(
        'STDIN (constant)'              => STDIN,
        'STDIN (fopen)'                 => fopen('php://stdin', 'rb'),
        'STDIN (php://fd/0)'            => fopen('php://fd/0', 'rb'),
        'STDOUT (constant)'             => STDOUT,
        'STDOUT (fopen)'                => fopen('php://stdout', 'wb'),
        'STDOUT (php://fd/1)'           => fopen('php://fd/1', 'wb'),
        'STDERR (constant)'             => STDERR,
        'STDERR (fopen)'                => fopen('php://stderr', 'wb'),
        'STDERR (php://fd/2)'           => fopen('php://fd/2', 'wb'),
        'Invalid stream (php://temp)'   => fopen('php://temp', 'wb'),
        'Invalid stream (php://input)'  => fopen('php://input', 'wb'),
        'Invalid stream (php://memory)' => fopen('php://memory', 'wb'),
        'File stream'                   => $closeMe = fopen(__FILE__, 'rb'),
    );

    foreach ($sampleStreams as $name => $stream) {
        echo "$name:\n";
        echo "- current value  : "; var_dump(sapi_windows_vt100_support($stream));
        echo "- enabling VT100 : "; var_dump(sapi_windows_vt100_support($stream, true));
        echo "- current value  : "; var_dump(sapi_windows_vt100_support($stream));
        echo "- disabling VT100: "; var_dump(sapi_windows_vt100_support($stream, false));
        echo "- current value  : "; var_dump(sapi_windows_vt100_support($stream));
    }

    fclose($closeMe);
    restoreVT100State($state);
}

function testToStdErr()
{
    ob_start();
    testToStdOut();
    $result = ob_get_contents();
    ob_end_clean();
    fwrite(STDERR, $result);
}
