# php-src docs

This is the home of the php-src internal documentation, hosted at
[php.github.io/php-src/](https://php.github.io/php-src/). It is in very early stages, but is
intended to become the primary place where new information about php-src is documented. Over time,
it is expected to replace various mediums like:

* https://www.phpinternalsbook.com/
* https://wiki.php.net/internals
* Blogs from contributors

## How to build

`python` 3 and `pip` are required.

```bash
pip install sphinx sphinx-design sphinxawesome-theme
make html
```

That's it! You can view the documentation under `./build/html/index.html` in your browser.

## Formatting

The files in this documentation are formatted using the
[``rstfmt``](https://github.com/dzhu/rstfmt) tool.

```bash
rstfmt -w 100 source
```

This tool is not perfect. It breaks on custom directives, so we might switch to either a fork or
something else in the future.
