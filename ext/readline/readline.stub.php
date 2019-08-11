<?php

/** @return string|false */
function readline(string $prompt = null) {}

/** @return mixed */
function readline_info(string $varname = null, string $newvalue = null) {}

function readline_add_history(string $prompt): bool {}

function readline_clear_history(): bool {}

#ifdef HAVE_HISTORY_LIST
function readline_list_history(): array {}
#endif

function readline_read_history(string $filename = null): bool {}

function readline_write_history(string $filename = null): bool {}

function readline_completion_function(callable $funcname): ?string {}


//ZEND_BEGIN_ARG_INFO_EX(arginfo_readline_completion_function, 0, 0, 1)
//	ZEND_ARG_INFO(0, funcname)
//ZEND_END_ARG_INFO()


#if HAVE_RL_CALLBACK_READ_CHAR
//ZEND_BEGIN_ARG_INFO_EX(arginfo_readline_callback_handler_install, 0, 0, 2)
//	ZEND_ARG_INFO(0, prompt)
//	ZEND_ARG_INFO(0, callback)
//ZEND_END_ARG_INFO()


//ZEND_BEGIN_ARG_INFO(arginfo_readline_callback_read_char, 0)
//ZEND_END_ARG_INFO()


//ZEND_BEGIN_ARG_INFO(arginfo_readline_callback_handler_remove, 0)
//ZEND_END_ARG_INFO()


//ZEND_BEGIN_ARG_INFO(arginfo_readline_redisplay, 0)
//ZEND_END_ARG_INFO()


#if HAVE_RL_ON_NEW_LINE
//ZEND_BEGIN_ARG_INFO(arginfo_readline_on_new_line, 0)
//ZEND_END_ARG_INFO()
#endif
#endif
