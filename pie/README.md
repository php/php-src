# PIE downloader

When building PHP, the `--with-pie` flag is enabled by default. This will
attempt to download the latest stable version of PIE, using `curl`, `wget`,
`fetch`, or a PHP script.

By default it will download PIE to `$prefix/bin/pie`. You can change
the target path, e.g. `--with-pie=/usr/local/bin`, which will cause PIE to be
downloaded to `/usr/local/bin/pie`.

If the `gh` CLI tool exists on the system, it will be used to verify that the
PIE that is downloaded was built within PHP's CI system. If not, the
`pie self-verify` command is used, but this has limited benefit.

To build PHP without PIE, supply the `--without-pie` flag.
