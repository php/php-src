# API adjustment to the old output control code

Everything now resides beneath the php_output namespace, and there's an API call
for every output handler op.

    Checking output control layers status:
        // Using OG()
        php_output_get_status();

    Starting the default output handler:
        // php_start_ob_buffer(NULL, 0, 1);
        php_output_start_default();

    Starting an user handler by zval:
        // php_start_ob_buffer(zhandler, chunk_size, erase);
        php_output_start_user(zhandler, chunk_size, flags);

    Starting an internal handler without context:
        // php_ob_set_internal_handler(my_php_output_handler_func_t, buffer_size, "output handler name", erase);
        php_output_start_internal(handler_name, handler_name_len, my_php_output_handler_func_t, chunk_size, flags);

    Starting an internal handler with context:
        // not possible with old API
        php_output_handler *h;
        h = php_output_handler_create_internal(handler_name, handler_name_len, my_php_output_handler_context_func_t, chunk_size, flags);
        php_output_handler_set_context(h, my_context, my_context_dtor);
        php_output_handler_start(h);

    Testing whether a certain output handler has already been started:
        // php_ob_handler_used("output handler name");
        php_output_handler_started(handler_name, handler_name_len);

    Flushing one output buffer:
        // php_end_ob_buffer(1, 1);
        php_output_flush();

    Flushing all output buffers:
        // not possible with old API
        php_output_flush_all();

    Cleaning one output buffer:
        // php_ob_end_buffer(0, 1);
        php_output_clean();

    Cleaning all output buffers:
        // not possible with old API
        php_output_clean_all();

    Discarding one output buffer:
        // php_ob_end_buffer(0, 0);
        php_output_discard();

    Discarding all output buffers:
        // php_ob_end_buffers(0);
        php_output_discard_all();

    Stopping (and dropping) one output buffer:
        // php_ob_end_buffer(1, 0)
        php_output_end();

    Stopping (and dropping) all output buffers:
        // php_ob_end_buffers(1, 0);
        php_output_end_all();

    Retrieving output buffers contents:
        // php_ob_get_buffer(zstring);
        php_output_get_contents(zstring);

    Retrieving output buffers length:
        // php_ob_get_length(zlength);
        php_output_get_length(zlength);

    Retrieving output buffering level:
        // OG(nesting_level);
        php_output_get_level();

    Issue a warning because of an output handler conflict:
        // php_ob_init_conflict("to be started handler name", "to be tested if already started handler name");
        php_output_handler_conflict(new_handler_name, new_handler_name_len, set_handler_name, set_handler_name_len);

    Registering a conflict checking function, which will be checked prior starting the handler:
        // not possible with old API, unless hardcoding into output.c
        php_output_handler_conflict_register(handler_name, handler_name_len, my_php_output_handler_conflict_check_t);

    Registering a reverse conflict checking function, which will be checked prior starting the specified foreign handler:
        // not possible with old API
        php_output_handler_reverse_conflict_register(foreign_handler_name, foreign_handler_name_len, my_php_output_handler_conflict_check_t);

    Facilitating a context from within an output handler callable with ob_start():
        // not possible with old API
        php_output_handler_hook(PHP_OUTPUT_HANDLER_HOOK_GET_OPAQ, (void *) &custom_ctx_ptr_ptr);

    Disabling of the output handler by itself:
        //not possible with old API
        php_output_handler_hook(PHP_OUTPUT_HANDLER_HOOK_DISABLE, NULL);

    Marking an output handler immutable by itself because of irreversibility of its operation:
        // not possible with old API
        php_output_handler_hook(PHP_OUTPUT_HANDLER_HOOK_IMMUTABLE, NULL);

    Restarting the output handler because of a CLEAN operation:
        // not possible with old API
        if (flags & PHP_OUTPUT_HANDLER_CLEAN) { ... }

    Recognizing by the output handler itself if it gets discarded:
        // not possible with old API
        if ((flags & PHP_OUTPUT_HANDLER_CLEAN) && (flags & PHP_OUTPUT_HANDLER_FINAL)) { ... }

## Output handler hooks

The output handler can change its abilities at runtime. Eg. the gz handler can
remove the CLEANABLE and REMOVABLE bits when the first output has passed through it;
or handlers implemented in C to be used with ob_start() can contain a non-global
context:

    PHP_OUTPUT_HANDLER_HOOK_GET_OPAQ
        pass a void*** pointer as second arg to receive the address of a pointer
        pointer to the opaque field of the output handler context
    PHP_OUTPUT_HANDLER_HOOK_GET_FLAGS
        pass a int* pointer as second arg to receive the flags set for the output handler
    PHP_OUTPUT_HANDLER_HOOK_GET_LEVEL
        pass a int* pointer as second arg to receive the level of this output handler
        (starts with 0)
    PHP_OUTPUT_HANDLER_HOOK_IMMUTABLE
        the second arg is ignored; marks the output handler to be neither cleanable
        nor removable
    PHP_OUTPUT_HANDLER_HOOK_DISABLE
        the second arg is ignored; marks the output handler as disabled

## Open questions

* Should the userland API be adjusted and unified?

Many bits of the manual (and very first implementation) do not comply with the
behaviour of the current (to be obsoleted) code, thus should the manual or the
behaviour be adjusted?
