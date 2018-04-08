<?php

function testToStdOut()
{
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
        'Not a stream'                  => 'foo',
        'Invalid stream (php://temp)'   => fopen('php://temp', 'wb'),
        'Invalid stream (php://input)'  => fopen('php://input', 'wb'),
        'Invalid stream (php://memory)' => fopen('php://memory', 'wb'),
        'File stream'                   => $closeMe = fopen(__FILE__, 'rb'),
    );

    foreach ($sampleStreams as $name => $stream) {
        echo "$name: "; var_dump(stream_isatty($stream));
    }

    fclose($closeMe);
}

function testToStdErr()
{
    ob_start();
    testToStdOut();
    $result = ob_get_contents();
    ob_end_clean();
    fwrite(STDERR, $result);
}
