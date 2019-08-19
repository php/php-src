<?php

/* {{{ head.c */
function header(string $string, bool $replace = true, int $http_response_code = null): void { }

function header_remove(string $name = null): void { }

function setrawcookie(strin $name, string $value = '', int $expire = 0, string $path = "", string $domain = "", bool $secure = false, bool $httponly = false): bool { }

function headers_sent(string &$file = null, int &$line = null) { }

function headers_list(): array { }

/* {{{ html.c */

function htmlspecialchars(string $string, int $flags = ENT_COMPAT | ENT_HTML401, string $encoding = 'UTF-8', bool $double_encode = true): string { }

function htmlspecialchars_decode(string $string, int $quote_style = null): string { }

function html_entity_decode(string $string, int $quote_style = null, string $charset = null): string { }

function htmlentities(string $string, int $quote_style = null, string $charset = null, bool $double_encode = true): string { }

function get_html_translation_table(int $table = null, int $quote_style = null, string $encoding = "UTF-8"): array { }

/* }}} */
