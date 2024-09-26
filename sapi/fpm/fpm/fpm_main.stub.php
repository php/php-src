<?php

/** @generate-class-entries */

function fastcgi_finish_request(bool $close_conn = true): bool {}

function apache_request_headers(): array {}

/** @alias apache_request_headers */
function getallheaders(): array {}

function fpm_get_status(): array|false {}
