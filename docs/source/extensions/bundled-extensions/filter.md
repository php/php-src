# ext/filter

Input filter support is implemented as SAPI hook which is called before external
variables are registered. `ext/filter` uses the hook to store original input and
populate `PG(http_globals)`.

## SAPI Input Filtering

Callers pass decoded external vars through `sapi_module.input_filter`. The input
source is detected via `PARSE_POST`,`PARSE_GET`, `PARSE_COOKIE`, `PARSE_STRING`,
`PARSE_ENV` or `PARSE_SERVER`. First `php_default_input_filter()` is registered,
then `php_sapi_filter()` replaces it in `ext/filter` while module initialising.

### Registration and Request Initialisation

Declared in `main/SAPI.h`:

```c
SAPI_API zend_result sapi_register_input_filter(
    unsigned int (*input_filter)(
        int arg,
        const char *var,
        char **val,
        size_t val_len,
        size_t *new_val_len
    ),
    unsigned int (*input_filter_init)(void)
);
```

If set, SAPI calls `input_filter_init` after activation for each request and
ignores its return value.

Only one input-filter slot exists. Registration replaces both callbacks without
chaining them. Replacing `php_sapi_filter()` disables `ext/filter`'s raw-input
storage and registration in `PG(http_globals)`.

### Callback Contract

`SAPI_INPUT_FILTER_FUNC` in `main/SAPI.h` declares the same signature:

- `arg`: input source.
- `var`: read-only variable name.
- `*val`: value, which may be modified or replaced.
- `val_len`: current value length.
- `new_val_len`: resulting length; may be `NULL`.

A non-zero return tells most callers to register the modified variable. Zero
rejects it or indicates that the callback registered it. `php_sapi_filter()`
uses zero after registration in `PG(http_globals)`.

`sapi_getenv()` ignores the return value and passes `NULL` for `new_val_len`.
Otherwise, set `new_val_len` before returning non-zero, even if unchanged.

`*val` ownership varies by caller. Some `PARSE_SERVER` values are owned by the
SAPI, so callbacks must not free it unconditionally.
