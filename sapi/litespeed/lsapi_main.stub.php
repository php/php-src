<?php

/** @generate-class-entries */

/**
 * @return array<string, string>
 * @alias apache_request_headers
 */
function litespeed_request_headers(): array {}

/**
 * @return array<string, string>
 * @alias apache_request_headers
 */
function getallheaders(): array {}

/**
 * @return array<string, string>
 * @refcount 1
 */
function apache_request_headers(): array {}

/**
 * @return array<string, string>|false
 * @refcount 1
 */
function litespeed_response_headers(): array|false {}

/**
 * @return array<string, string>|false
 * @alias litespeed_response_headers
 */
function apache_response_headers(): array|false {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function apache_get_modules(): array {}

function litespeed_finish_request(): bool {}
