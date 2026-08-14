# php-src docs

This is the home of the php-src internal documentation, hosted at
[php.github.io/php-src/](https://php.github.io/php-src/). It is in very early
stages, but is intended to become the primary place where new information about
php-src is documented. Over time, it is expected to replace various mediums
like:

* https://www.phpinternalsbook.com/
* https://wiki.php.net/internals
* Blogs from contributors

## How to build

`python` 3 and `pip` are required.

```bash
cd docs
# Recommended: Initialize and activate a Python virtual environment
pip install --upgrade pip
pip install -r requirements.txt
make html
```

That's it! You can view the documentation under `./build/html/index.html` in
your browser.

## Formatting

Formatting is temporarily not enforced during the Markdown migration.
