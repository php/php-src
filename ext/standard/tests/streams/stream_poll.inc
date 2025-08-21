<?php

function new_stream_poll(): StreamPollContext {
    $backend = getenv('STREAM_POLL_TEST_BACKEND');
    return stream_poll_create($backend === false ? STREAM_POLL_BACKEND_AUTO : $backend);
}
