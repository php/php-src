# Vulnerability Disclosure Policy

*This document was originally published at <https://wiki.php.net/security>.*

## Introduction

For the sake of our users, we classify some of the issues found in PHP
as "security issues". This document is intended to explain which issues
are thus classified, how we handle those issues and how to report them.

## Classification

We classify as security issues bugs that:

- allow users to execute unauthorized actions
- cross security boundaries
- access data that is not intended to be accessible
- severely impact accessibility or performance of the system

The purpose of this classification is to alert the users and the
developers about the bugs that need to be prioritized in their handling.

We define three categories of security issues, by their severity,
described below. Please note that this categorization is in many aspects
subjective, so it ultimately relies on the judgement of the PHP
developers.

### High severity

These issues may allow:

- third party to compromise any, or most installations of PHP
- the execution of arbitrary code
- disabling the system completely
- access to any file a local PHP user can access.

The issue can be triggered on any, or on most typical installations, and
does not require exotic and non-recommended settings to be triggered.

This category also includes issues that can be triggered in code or
functions known to be frequently used (session, json, mysql, openssl,
etc.) during typical usage, and that require settings or configurations
that may not be strictly the best practice, but are commonly used.

This category also may include issues that require special code or code
pattern if such code or pattern is present in many popular libraries.

This kind of issues usually requires a CVE report.

### Medium severity

These issues may have the same potential to compromise an installation
as a high severity issue, but may also require:

- an extension that is not commonly used
- a particular type of configuration that is used only in narrow
  specific circumstances
- relies on old version of a third-party library being used
- code, or patterns of code, that are known to be used infrequently
- code that is very old, or extremely uncommon (and so is used
  infrequently)

This kind of issues usually will have a CVE number, unless the required
configuration is particularly exotic to the point it's not practically
usable.

### Low severity

This issue allows theoretical compromise of security, but practical
attack is usually impossible or extremely hard due to common practices
or limitations that are virtually always present or imposed.

This also includes problems with configuration, documentation, and other
non-code parts of the PHP project that may mislead users, or cause them
to make their system, or their code less secure.

Issues that can trigger unauthorized actions that do not seem to be
useful for any practical attack can also be categorized as low severity.

Security issues, that are present only in unstable branches, belong to
this category, too. Any branch that has no stable release, is per se not
intended for the production use.

Low severity issues usually do not need to have CVE and may, at the
discretion of the PHP developers, be disclosed publicly before the fix
is released or available.

### Not a security issue

We do not classify as a security issue any issue that:

- requires invocation of specific code, which may be valid but is
  obviously malicious
- requires invocation of functions with specific arguments, which may
  be valid but are obviously malicious
- requires specific actions to be performed on the server, which are
  not commonly performed, or are not commonly permissible for the user
  (uid) executing PHP
- requires privileges superior to that of the user (uid) executing PHP
- requires the use of debugging facilities - ex. xdebug, var_dump
- requires the use of settings not recommended for production - ex.
  error reporting to output
- requires the use of non-standard environment variables - ex.
  USE_ZEND_ALLOC
- requires the use of non-standard builds - ex. obscure embedded
  platform, not commonly used compiler
- requires the use of code or settings known to be insecure
- requires the use of FFI
- requires an open_basedir bypass

## Handling issues

High and medium severity fixes are merged into a private security repository,
and then merged to the main repository before the release is tagged.

Low severity fixes are merged immediately after the fix is available and
handled like all regular bugs are handled consequently. However, release
managers may choose to pull those fixes into the RC branch after the
branch is created, and also backport them into a security-only release
branch.

## FAQ

### How do I report a security issue?

Please report security vulnerabilities on GitHub at:
<https://github.com/php/php-src/security/advisories/new>

If for some reason you cannot use the form at GitHub, or you need to talk to
somebody about a PHP security issue that might not be a bug report, please write
to <security@php.net>.

Vulnerability reports remain private until published. When published, you will
be credited as a contributor, and your contribution will reflect the MITRE
Credit System.

### What do you consider a responsible disclosure?

Please report the issue as described above. Please communicate with
the developers about when the fix will be released - usually it's the
next monthly release after the bug was reported. Some issues can take
longer. After the fix is released (releases usually happen on Thursdays)
please feel free to disclose the issue as you see fit.

### What if I think it's a security issue but the developers disagree?

Please read the above and try to explain to us why it fits the
description.

### What if the developers still don't think it's a security issue?

We'll have to agree to disagree.

### The bug I submitted was classified as "not a security issue." You don't believe it's real?

It has nothing to do with the bug being real or its importance to
you. It just means it does not fit our specific definitions for issues
that we will handle in a special way. We fix a lot of non-security bugs
and pull requests are always welcome.

### But you classified bug #424242 as a security issue, but not this one?!

Each bug usually has its aspects, if a short discussion does not
yield agreement we'd rather do more fixing and less arguing.

### Do you pay bounties for security issues?

PHP is a volunteer project. We have no money, thus we can't pay bounties.
