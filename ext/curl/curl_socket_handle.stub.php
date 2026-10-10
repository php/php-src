<?php

/** @generate-class-entries */

/**
 * Identity of one socket in a multi handle's connection pool, from the first
 * report to CURL_POLL_REMOVE; a pooled connection reused later gets a new one.
 * Created by the extension's socket callback only, so userland sees it
 * through operations and registrations.
 * @strict-properties
 * @not-serializable
 */
final class CurlSocketPollWeakHandle implements Io\Poll\WeakHandle
{
    private function __construct() {}

    public function isValid(): bool {}
}
