<?php

function filter_errors_skipif($needle) {
    if (!function_exists('fnmatch')) {
        die('skip fnmatch() not available');
    }
    $filters = stream_get_filters();
    foreach($filters as $filter) {
        if (fnmatch($filter, $needle)) return;
    }
    die("skip $needle not available");
}

function filter_errors_test($filter, $data) {

    echo "test filtering of buffered data\n";

    $stream = fopen('php://memory', 'wb+');

    fwrite($stream, ".\r\n$data");
    fseek($stream, 0, SEEK_SET);
    stream_get_line($stream, 8192, "\r\n");

    $f = stream_filter_append($stream, $filter);

    echo "test filtering of non buffered data\n";

    $stream = fopen('php://memory', 'wb+');

    fwrite($stream, "$data");
    fseek($stream, 0, SEEK_SET);

    stream_filter_append($stream, $filter);
    stream_get_contents($stream);

}
