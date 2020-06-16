<?php

/** @generate-function-entries */

function litespeed_request_headers(): array {}

/** @alias litespeed_request_headers */
function getallheaders(): array {}

/** @alias litespeed_request_headers */
function apache_request_headers(): array {}

function litespeed_response_headers(): array|false {}

/** @alias litespeed_response_headers */
function apache_response_headers(): array|false {}

function apache_get_modules(): array {}

function litespeed_finish_request(): bool {}
