# Streams

PHP streams provide one byte-stream abstraction for files, sockets, memory and
wrapper-backed sources. Core code can therefore avoid source-specific I/O
paths. The stream layer also coordinates buffering, filters, contexts and
resource lifetime.

## Basic Operations

A `php_stream *` has a similar role to a `FILE *`. Normal callers use the API
in `main/php_streams.h` rather than accessing its fields. The main operations
are:

```c
PHPAPI ssize_t php_stream_read(
    php_stream *stream, char *buf, size_t count
);
PHPAPI ssize_t php_stream_write(
    php_stream *stream, const char *buf, size_t count
);
PHPAPI ssize_t php_stream_printf(
    php_stream *stream, const char *fmt, ...
);
PHPAPI bool php_stream_eof(php_stream *stream);
PHPAPI int php_stream_getc(php_stream *stream);
PHPAPI char *php_stream_get_line(
    php_stream *stream, char *buf, size_t maxlen, size_t *returned_len
);
PHPAPI int php_stream_flush(php_stream *stream);
PHPAPI int php_stream_seek(
    php_stream *stream, zend_off_t offset, int whence
);
PHPAPI zend_off_t php_stream_tell(const php_stream *stream);
#define php_stream_close(stream) \
    php_stream_free((stream), PHP_STREAM_FREE_CLOSE)
```

These mostly follow their stdio equivalents. Reads and writes return `ssize_t`,
so a negative result can report failure. Positions and offsets use
`zend_off_t`.

Use these functions rather than calling `stream->ops` directly. The stream
layer maintains buffering, filters and its logical position around the
underlying operations.

Use `php_stream_supports_lock()` before `php_stream_lock()` when locking is
required. Both delegate to the implementation's `set_option` callback.

## Opening Streams

Use `php_stream_open_wrapper()` for paths handled by stream wrappers:

```c
zend_string *opened_path = NULL;
php_stream *stream = php_stream_open_wrapper(
    path, mode, options, &opened_path
);
```

`options` is a bitmask. Common values are:

- `USE_PATH`: search `PG(include_path)`.
- `IGNORE_URL`: disallow URL wrappers.
- `REPORT_ERRORS`: report failures through the stream error API.
- `STREAM_MUST_SEEK`: return a seekable stream or fail.
- `STREAM_WILL_CAST`: prepare a wrapper stream for a later cast.
- `STREAM_OPEN_PERSISTENT`: require a persistent stream.

Pass `NULL` instead of `&opened_path` when the resolved path is not needed.
Otherwise, release a returned path with `zend_string_release()`.
`php_stream_open_wrapper_ex()` additionally accepts a `php_stream_context *`.

Helpers for plain files, descriptors, pipes and temporary files are declared
in `main/streams/php_stream_plain_wrapper.h`. Socket helpers are declared in
`main/php_network.h`.

## Copying Streams

Use the following interfaces to copy between streams or into memory:

```c
zend_result php_stream_copy_to_stream_ex(
    php_stream *src,
    php_stream *dest,
    size_t maxlen,
    size_t *copied
);

zend_string *php_stream_copy_to_mem(
    php_stream *src,
    size_t maxlen,
    bool persistent
);
```

Pass `PHP_STREAM_COPY_ALL` to copy until EOF. `copied` may be `NULL` when the
length is not needed. `php_stream_copy_to_stream()` is deprecated; use the
`_ex` form so failure is distinguishable from the byte count.

Release a non-`NULL` string returned by `php_stream_copy_to_mem()` with
`zend_string_release()`. Its `persistent` argument controls the string's
allocation.

## Seeking

`php_stream_seek()` accounts for buffered data and can emulate a forward
`SEEK_CUR` by reading. Arbitrary seeks require the stream implementation to
provide a `seek` operation.

`STREAM_MUST_SEEK` makes `php_stream_open_wrapper()` copy a non-seekable source
to a temporary stream. When this occurs, opening may block until the source is
exhausted, and writes to the temporary stream do not affect the source.

`php_stream_make_seekable()` performs the same conversion explicitly:

```c
php_stream *seekable;
php_stream_make_seekable_status status = php_stream_make_seekable(
    stream, &seekable, PHP_STREAM_NO_PREFERENCE
);
```

Use `PHP_STREAM_PREFER_STDIO` to prefer a file-backed replacement, or
`PHP_STREAM_FORCE_CONVERSION` to replace an already seekable stream.

- `PHP_STREAM_UNCHANGED`: the returned stream is the original stream.
- `PHP_STREAM_RELEASED`: the returned stream replaces the closed original.
- `PHP_STREAM_FAILED`: conversion failed and the original remains valid.
- `PHP_STREAM_CRITICAL`: conversion failed; close the original stream.

After either success result, use the returned stream.

> [!WARNING]
> Never call `php_stream_make_seekable()` for a stream referenced by a resource.
> It may close the original while the resource still points to it.

## Casting Streams

`php_stream_cast()` exposes a compatible underlying handle:

```c
PHPAPI zend_result php_stream_cast(
    php_stream *stream,
    int castas,
    void **result,
    int show_err
);
```

The base cast types are:

- `PHP_STREAM_AS_STDIO`: a `FILE *`.
- `PHP_STREAM_AS_FD`: a file descriptor.
- `PHP_STREAM_AS_SOCKETD`: a socket descriptor.
- `PHP_STREAM_AS_FD_FOR_SELECT`: a descriptor for `select()`.
- `PHP_STREAM_AS_FD_FOR_COPY`: a `php_io_fd` for internal copying.

A non-zero `show_err` reports a warning when casting fails.
`php_stream_can_cast()` queries support by passing a `NULL` result, while
`php_stream_is()` only compares the operations table.

Avoid `PHP_STREAM_CAST_TRY_HARD` unless consuming the source into a temporary
stream is acceptable. `PHP_STREAM_CAST_RELEASE` invalidates the stream after a
successful cast.

Where supported, a stdio cast may create a `FILE *` with `fopencookie()` rather
than expose an existing handle.

Do not interleave access through a cast handle with `php_stream_*()` calls.
Their separate buffering can desynchronise positions or lose buffered data.

## Stream Implementations

A `php_stream` holds common state and a `php_stream_ops` table. Implementations
store their own state in `stream->abstract` and allocate the stream with
`php_stream_alloc()`.

For a normal data stream, `write`, `read`, `close` and `flush` are mandatory.
`seek`, `cast`, `stat` and `set_option` are optional:

```c
static const php_stream_ops my_ops = {
    my_write,
    my_read,
    my_close,
    my_flush,
    "my stream",
    my_seek,
    NULL, /* cast */
    NULL, /* stat */
    NULL, /* set_option */
};
```

The callbacks have several important contracts:

- `read` and `write` return a byte count or a negative value on failure.
- `read` sets `stream->eof` when the source reaches its final EOF.
- `seek` writes the new position and returns zero on success.
- `close` releases owned state in `stream->abstract` and honours `close_handle`
  for any underlying handle.

Allocate a non-persistent stream as follows:

```c
php_stream *stream = php_stream_alloc(&my_ops, state, NULL, mode);
```

The third argument is a persistent identifier, not a Boolean flag. When it is
non-`NULL`, owned implementation state must use matching persistent allocation.
Use `php_stream_is_persistent()` when freeing that state. Supply a valid
fopen-style `mode`; streams retain it for casts and other stream operations.

Use `php_stream_to_zval()` when returning a stream as a PHP resource. Once
exposed, its resource controls the stream's lifetime.

Build PHP with `--enable-debug` while developing an implementation. The
`STREAMS_*` call-site macros then help diagnose allocation and lifetime errors.
Current examples are available in `main/streams/plain_wrapper.c` and bundled
extensions such as `ext/bz2/bz2.c`.
