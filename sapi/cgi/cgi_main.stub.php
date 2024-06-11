<?php

/** @generate-class-entries */

function apache_child_terminate(): void {}

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
 * @return array<string, string>
 * @refcount 1
 */
function apache_response_headers(): array {}
