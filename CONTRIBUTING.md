# Contributing to PHP

Anybody who programs in PHP can be a contributing member of the community that
develops and deploys it; the task of deploying PHP, documentation and associated
websites is a never ending one. With every release, or release candidate comes a
wave of work, which takes a lot of organization and co-ordination.

You don't need any special access to download, build, debug and begin submitting
PHP or PECL code, tests or documentation. Once you've followed this guide and
had several contributions accepted, commit privileges are often quickly granted.

## Index

* [Pull requests](#pull-requests)
* [Filing bugs](#filing-bugs)
* [Feature requests](#feature-requests)
* [Writing tests](#writing-tests)
* [Writing documentation](#writing-documentation)
* [Getting help](#getting-help)
* [PHP source code directory structure](#php-source-code-directory-structure)
* [PHP internals](#php-internals)
* [Submitting patches](#submitting-patches)
  * [PHP documentation patches](#php-documentation-patches)
  * [How to create a patch?](#how-to-create-a-patch)
* [Checklist for submitting contribution](#checklist-for-submitting-contribution)
* [PECL extensions](#pecl-extensions)
* [What happens after submitting contribution?](#what-happens-after-submitting-contribution)
* [What happens when your contribution is applied?](#what-happens-when-your-contribution-is-applied)

## Pull requests

PHP welcomes pull requests to [add tests](#writing-tests), fix bugs and to
implement RFCs. Please be sure to include tests as appropriate!

If you are fixing a bug, then please submit your PR against the lowest actively
supported branch of PHP that the bug affects (only green branches on
[the supported version page](https://php.net/supported-versions.php) are
supported). For example, at the time of writing, the lowest supported version is
PHP 7.2, which corresponds to the `PHP-7.2` branch in Git. Please also make sure
you add a link to the PR in the bug on [the bug tracker](https://bugs.php.net/).

Pull requests implementing RFCs should be submitted against `master`.

Pull requests should *never* be submitted against `PHP-x.y.z` branches, as these
are only used for release management.

If your pull request exhibits conflicts with the base branch, please resolve
them by using `git rebase` instead of `git merge`.

Fork the official PHP repository and send a pull request. A notification will be
sent to the pull request mailing list. Sending a note to PHP Internals list
(internals@lists.php.net) may help getting more feedback and quicker turnaround.
You can also add pull requests to [bug reports](https://bugs.php.net/).

## Filing bugs

Bugs can be filed on the [PHP bug tracker](https://bugs.php.net/). If this is
the first time you've filed a bug, we suggest reading the
[guide to reporting a bug](https://bugs.php.net/how-to-report.php).

Where possible, please include a self-contained reproduction case!

## Feature requests

Feature requests are generally submitted in the form of
[Requests for Comments (RFC)](https://wiki.php.net/rfc/howto), ideally
accompanied by [pull requests](#pull-requests). You can find the extremely large
list of RFCs that have been previously considered on the
[PHP Wiki](https://wiki.php.net/rfc).

To create a RFC, discuss it with the extension maintainer, and discuss it on the
development mailing list internals@lists.php.net. RFC Wiki accounts can be
requested on https://wiki.php.net/start?do=register. PHP extension maintainers
can be found in the [EXTENSIONS](/EXTENSIONS) file in the PHP source code
repository. Mailing list subscription is explained on the
[mailing lists page](https://php.net/mailing-lists.php).

You may also want to read
[The Mysterious PHP RFC Process](https://blogs.oracle.com/opal/entry/the_mysterious_php_rfc_process)
for additional notes on the best way to approach submitting an RFC.

## Writing tests

We love getting new tests! PHP is a huge project and improving code coverage is
a huge win for every PHP user.

[Our QA site includes a page detailing how to write test cases.](https://qa.php.net/write-test.php)

## Writing documentation

There are two ways to contribute to the PHP manual. You can edit the manual and
send patches anonymously via [the online editor](https://edit.php.net/), or you
can check the XML source out from Subversion and edit that and build it
[per the instructions on the documentation site](http://doc.php.net/tutorial/).
Patches created that way should be sent to the
[documentation mailing list](mailto:phpdoc@lists.php.net).

## Getting help

If you are having trouble contributing to PHP, or just want to talk to a human
about what you're working on, you can contact us via the
[internals mailing list](mailto:internals@lists.php.net), or the
[documentation mailing list](mailto:phpdoc@lists.php.net) for documentation
issues.

Although not a formal channel, you can also find a number of core developers on
the #php.pecl channel on [EFnet](http://www.efnet.org/). Similarly, many
documentation writers can be found on #php.doc. Windows development IRC channel
is available at #winphp-dev on FreeNode.

## PHP source code directory structure

PHP source code also includes several files generated during development and
several parts where maintenance is happening upstream in their respective
locations.

```bash
<php-src>/
 ├─ .git/                           # Git configuration and source directory
 └─ TSRM/                           # Thread Safe Resource Manager
    └─ m4/                          # https://github.com/autoconf-archive/autoconf-archive
       └─ ax_func_which_gethostbyname_r.m4
    └─ ...
 └─ Zend/                           # Zend Engine
    ├─ zend_vm_execute.h            # Generated by `Zend/zend_vm_gen.php`
    ├─ zend_vm_opcodes.c            # Generated by `Zend/zend_vm_gen.php`
    ├─ zend_vm_opcodes.h            # Generated by `Zend/zend_vm_gen.php`
    └─ ...
 ├─ appveyor/                       # Appveyor CI service files
 └─ build/                          # *nix build system files
    ├─ ax_check_compile_flag.m4     # https://github.com/autoconf-archive/autoconf-archive
    ├─ ax_gcc_func_attribute.m4     # https://github.com/autoconf-archive/autoconf-archive
    ├─ config.guess                 # https://git.savannah.gnu.org/cgit/config.git
    ├─ config.sub                   # https://git.savannah.gnu.org/cgit/config.git
    ├─ libtool.m4                   # https://git.savannah.gnu.org/cgit/libtool.git
    ├─ ltmain.sh                    # https://git.savannah.gnu.org/cgit/libtool.git
    ├─ shtool                       # https://www.gnu.org/software/shtool/
    └─ ...
 └─ ext/                            # PHP core extensions
    └─ bcmath/
       ├─ libbcmath/                # Forked and maintained in php-src
       └─ ...
    └─ curl/
       ├─ sync-constants.php        # The curl symbols checker
       └─ ...
    └─ date/
       └─ lib/                      # Bundled datetime library https://github.com/derickr/timelib
          ├─ parse_date.c           # Generated by re2c 0.15.3
          ├─ parse_iso_intervals.c  # Generated by re2c 0.15.3
          └─ ...
       └─ ...
    └─ fileinfo/
       ├─ libmagic/                 # Modified libmagic https://github.com/file/file
       ├─ data_file.c               # Generated by `ext/fileinfo/create_data_file.php`
       ├─ libmagic.patch            # Modifications patch from upstream libmagic
       ├─ magicdata.patch           # Modifications patch from upstream libmagic
       └─ ...
    └─ gd/
       ├─ libgd/                    # Bundled and modified GD library https://github.com/libgd/libgd
       └─ ...
    └─ mbstring/
       ├─ libmbfl/                  # Forked and maintained in php-src
       ├─ unicode_data.h            # Generated by `ext/mbstring/ucgendat/ucgendat.php`
       └─ ...
    └─ pcre/
       ├─ pcre2lib/                 # https://www.pcre.org/
       └─ ...
    └─ pdo_mysql/
       ├─ php_pdo_mysql_sqlstate.h  # Generated by `ext/pdo_mysql/get_error_codes.php`
       └─ ...
    └─ skeleton/                    # Skeleton for developing new extensions with `ext/ext_skel.php`
       └─ ...
    └─ standard/
       └─ html_tables/
          ├─ mappings/              # https://www.unicode.org/Public/MAPPINGS/
          └─ ...
       ├─ credits_ext.h             # Generated by `scripts/dev/credits`
       ├─ credits_sapi.h            # Generated by `scripts/dev/credits`
       ├─ html_tables.h             # Generated by `ext/standard/html_tables/html_table_gen.php`
       └─ ...
    └─ tokenizer/
       ├─ tokenizer_data.c          # Generated by `ext/tokenizer/tokenizer_data_gen.sh`
       └─ ...
    └─ xmlrpc/
       ├─ libxmlrpc/                # Forked and maintained in php-src
       └─ ...
    └─ zend_test                    # For testing internal APIs. Not needed for regular builds.
       └─ ...
    └─ zip/                         # Bundled https://github.com/pierrejoye/php_zip
       └─ ...
    └─ ...
 └─ main/                           # Binding that ties extensions, SAPIs, and engine together
    ├─ streams/                     # Streams layer subsystem
    ├─ php_version.h                # Generated by release managers using `configure`
    └─ ...
 ├─ pear/                           # PEAR installation
 └─ sapi/                           # PHP SAPI modules
    └─ cli/
       ├─ mime_type_map.h           # Generated by `sapi/cli/generate_mime_type_map.php`
       └─ ...
    └─ ...
 ├─ scripts/                        # php-config, phpize and internal development scripts
 ├─ tests/                          # Core features tests
 ├─ travis/                         # Travis CI service files
 └─ win32/                          # Windows build system files
    ├─ cp_enc_map.c                 # Generated by `win32/cp_enc_map_gen.exe`
    └─ ...
 └─ ...
```

## PHP internals

For information on PHP internal C functions see
[PHP at the Core: A Hacker's Guide](https://php.net/internals), though this is
considered incomplete. Various external resources can be found on the web. A
standard printed reference is the book "Extending and Embedding PHP" by Sara
Golemon.

## Submitting patches

GitHub pull requests are the preferred way to propose PHP patch. If you are not
using GitHub, attach your patch to a PHP bug and consider sending a notification
email about the change to internals@lists.php.net. If the bug is for an
extension, also CC the extension maintainer. Explain what has been changed by
your patch. Test scripts should be included.

Please make the mail subject prefix `[PATCH]`. If attaching a patch, ensure it
has a file extension of `.txt`. This is because only MIME attachments of type
`text/*` are accepted.

If you are fixing broken functionality in PHP C source code first create a bug
or identify an existing bug at [bugs.php.net](https://bugs.php.net). A bug can
be used to track the patch progress and prevent your changes getting lost in the
PHP mail archives.

### PHP documentation patches

If you are fixing incorrect PHP documentation first create a bug or identify an
existing bug at [bugs.php.net](https://bugs.php.net). A bug can be used to track
the patch progress and prevent your changes getting lost in the PHP mail
archives.

If your change is large, then first discuss it with the mailing list
phpdoc@lists.php.net. Subscription is explained on the
[mailing lists page](https://php.net/mailing-lists.php).

Attach the patch to the PHP bug and consider sending a notification email about
the change to phpdoc@lists.php.net. Explain what has been fixed/added/changed by
your patch.

### How to create a patch?

PHP and most PECL packages use Git for revision control. Read
[Git access page](https://php.net/git.php) for help on using Git to get and
build PHP source code. We recommend to look at our
[workflow](https://wiki.php.net/vcs/gitworkflow) and our
[FAQ](https://wiki.php.net/vcs/gitfaq).

Generally we ask that bug fix patches work on the current stable PHP development
branches and on `master`. New PHP features only need to work on `master`.

After modifying the source see
[Creating new test files](https://qa.php.net/write-test.php) for how to test.
Submitting test scripts helps us to understand what functionality has changed.
It is important for the stability and maintainability of PHP that tests are
comprehensive.

After testing is finished, create a patch file using the command:

    git diff > your_patch.txt

For ease of review and later troubleshooting, submit individual patches for each
bug or feature.

## PECL extensions

If you are fixing broken functionality in a [PECL](https://pecl.php.net)
extension then create a bug or identify an existing bug at
[bugs.php.net](https://bugs.php.net). A bug can be used to track the change
progress and prevent your changes getting lost in the PHP mail archives. Some
PECL extensions have their own bug tracker locations and different contributing
procedures.

If your change is large then create a
[Request for Comments (RFC)](https://wiki.php.net/rfc), discuss it with the
extension maintainer, and discuss it on the development mailing list
pecl-dev@lists.php.net depending on the extension. PECL mailing list
subscription is explained on the
[PECL support page](https://pecl.php.net/support.php).

Update any open bugs and add a link to the source of your change. Send the patch
or pointer to the bug to pecl-dev@lists.php.net. Also CC the extension
maintainer. Explain what has been changed by your patch. Test scripts should be
included.

## Checklist for submitting contribution

- Read [CODING_STANDARDS](/CODING_STANDARDS) before you start working.
- Update git source just before running your final `diff` and before testing.
- Add in-line comments and/or have external documentation ready. Use only
  `/* */` style comments, not `//`.
- Create test scripts for use with `make test`.
- Run `make test` to check your change doesn't break other features.
- Rebuild PHP with `--enable-debug` which will show some kinds of memory errors
  and check the PHP and web server error logs after running your PHP tests.
- Rebuild PHP with `--enable-maintainer-zts` to check your change compiles on
  multi-threaded web servers.
- Review the change once more just before submitting it.

## What happens after submitting contribution?

If your change is easy to review and obviously has no side-effects, it might be
committed relatively quickly.

Because PHP is a volunteer-driven effort more complex changes will require
patience on your side. If you do not receive feedback in a few days, consider
bumping. Before doing this think about these questions:

- Did I send the patch to the right mailing list?
- Did I review the mailing list archives to see if these kind of changes had
  been discussed before?
- Did I explain my change clearly?
- Is my change too hard to review? Because of what factors?

## What happens when your contribution is applied?

Your name will likely be included in the Git commit log. If your change affects
end users, a brief description and your name might be added to the [NEWS](/NEWS)
file.

Thank you for contributing to PHP!
