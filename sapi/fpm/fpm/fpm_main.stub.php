<?php

/** @generate-class-entries */

function fastcgi_finish_request(): bool {}

/**
 * @return array<string, string>
 * @refcount 1
 */
function apache_request_headers(): array {}

/**
 * @return array<string, string>
 * @alias apache_request_headers
 */
function getallheaders(): array {}

/**
 * @return array<string, int|string|array>|false
 * @refcount 1
 */
function fpm_get_status(): array|false {}
