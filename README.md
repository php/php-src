<div align="center">
    <a href="https://php.net">
        <img
            alt="PHP"
            src="https://www.php.net/images/logos/new-php-logo.svg"
            width="150">
    </a>
</div>

# The PHP Interpreter

PHP is a popular general-purpose scripting language that is especially suited to
web development. Fast, flexible and pragmatic, PHP powers everything from your
blog to the most popular websites in the world. PHP is distributed under the PHP
License v3.01.

[![Build status](https://travis-ci.org/php/php-src.svg?branch=master)](https://travis-ci.org/php/php-src)
[![Build status](https://ci.appveyor.com/api/projects/status/meyur6fviaxgdwdy?svg=true)](https://ci.appveyor.com/project/php/php-src)
[![Build Status](https://dev.azure.com/phpazuredevops/php/_apis/build/status/php.php-src?branchName=PHP-7.4)](https://dev.azure.com/phpazuredevops/php/_build/latest?definitionId=1&branchName=PHP-7.4)

## Documentation

The PHP manual is available at [php.net/docs](https://php.net/docs).

## Installation

### Prebuilt packages and binaries

Prebuilt packages and binaries can be used to get up and running fast with PHP.

For Windows, the PHP binaries can be obtained from
[windows.php.net](https://windows.php.net). After extracting the archive the
`*.exe` files are ready to use.

For other systems, see the [installation chapter](https://php.net/install).

### Building PHP source code

*For Windows, see [Build your own PHP on Windows](https://wiki.php.net/internals/windows/stepbystepbuild_sdk_2).*

PHP uses autotools on Unix systems to configure the build:

    ./buildconf
    ./configure [options]

*See `./configure -h` for configuration options.*

    make [options]

*See `make -h` for make options.*

The `-j` option shall set the maximum number of jobs `make` can use for the
build:

    make -j4

Shall run `make` with a maximum of 4 concurrent jobs: Generally the maximum
number of jobs should not exceed the number of cores available.

## Testing PHP source code

PHP ships with an extensive test suite, the command `make test` is used after
successful compilation of the sources to run this test suite.

It is possible to run tests using multiple cores by setting `-jN` in
`TEST_PHP_ARGS`:

    make TEST_PHP_ARGS=-j4 test

Shall run `make test` with a maximum of 4 concurrent jobs: Generally the maximum
number of jobs should not exceed the number of cores available.

The [qa.php.net](https://qa.php.net) site provides more detailed info about
testing and quality assurance.

## Installing PHP built from source

After a successful build (and test), PHP may be installed with:

    make install

Depending on your permissions and prefix, `make install` may need super user
permissions.

## PHP extensions

Extensions provide additional functionality on top of PHP. PHP consists of many
essential bundled extensions. Additional extensions can be found in the PHP
Extension Community Library - [PECL](https://pecl.php.net).

## Contributing

The PHP source code is located in the Git repository at
[git.php.net](https://git.php.net). Contributions are most welcome by forking
the [GitHub mirror repository](https://github.com/php/php-src) and sending a
pull request.

Discussions are done on GitHub, but depending on the topic can also be relayed
to the official PHP developer mailing list internals@lists.php.net.

New features require an RFC and must be accepted by the developers. See
[Request for comments - RFC](https://wiki.php.net/rfc) and
[Voting on PHP features](https://wiki.php.net/rfc/voting) for more information
on the process.

Bug fixes **do not** require an RFC but require a bug tracker ticket. Open a
ticket at [bugs.php.net](https://bugs.php.net) and reference the bug id using
`#NNNNNN`.

    Fix #55371: get_magic_quotes_gpc() throws deprecation warning

    After removing magic quotes, the get_magic_quotes_gpc function caused a
    deprecated warning. get_magic_quotes_gpc can be used to detect the
    magic_quotes behavior and therefore should not raise a warning at any time.
    The patch removes this warning.

Pull requests are not merged directly on GitHub. All PRs will be pulled and
pushed through [git.php.net](https://git.php.net). See
[Git workflow](https://wiki.php.net/vcs/gitworkflow) for more details.

### Guidelines for contributors

See further documents in the repository for more information on how to
contribute:

- [Contributing to PHP](/CONTRIBUTING.md)
- [PHP coding standards](/CODING_STANDARDS.md)
- [Mailinglist rules](/docs/mailinglist-rules.md)
- [PHP release process](/docs/release-process.md)

## Credits

For the list of people who've put work into PHP, please see the
[PHP credits page](https://php.net/credits.php).
