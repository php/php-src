# Vulnerability Disclosure Policy

For the sake of our users, we classify some of the issues found in PHP as
"security issues". This document explains how to report such issues, which
issues are thus classified, and how we handle them.

# Reporting Security Issues

Please report security vulnerabilities on GitHub at:
<https://github.com/php/php-src/security/advisories/new>

If for some reason you cannot use the form at GitHub, or you need to talk to
somebody about a PHP security issue that might not be a bug report, please write
to <security@php.net>.

Vulnerability reports remain private until published. When published, you will
be credited as a contributor, and your contribution will reflect the MITRE
Credit System.

When creating reports, please **skip** the theatrics. Drop the impact essay,
send a short reproducer with the few lines that matter, and make each point
once. This allows us to triage and respond to your report quickly.

# Classification

We classify as security issues bugs that:

- allow users to execute unauthorized actions
- cross security boundaries
- access data that is not intended to be accessible
- severely impact availability or performance of the system

The purpose of this classification is to alert the users and the developers
about the bugs that need to be prioritized in their handling.

The severity of each security issue is assessed using the [Common Vulnerability
Scoring System version 4 (CVSS v4)](https://www.first.org/cvss/v4-0/), following
its specification as closely as possible. The resulting score and vector are
published in the security advisory. Whether an issue is classified as a security
issue at all is decided by the PHP developers based on the criteria above and
the list below.

## Not a Security Issue

Issues commonly reported that are _not_ considered security issues include (but
are not limited to):

- Invocation of specially crafted, malicious code intended to cause memory
  violations. This commonly includes malicious error handlers, destructors or
  `__toString()` functions. PHP does not offer sandboxing, and the execution of
  untrusted code is always considered unsafe. Such issues are bugs, but not
  security issues. They may still be reported, though please avoid reporting
  the known issues.

- Passing malicious arguments to functions clearly not intended to receive
  unsanitized values, e.g. `mysqli_query()`. `escapeshellarg()` on the other
  hand should clearly be hardened against unsafe inputs.

- Issues that require specific actions to be performed on the server which are
  not commonly performed, or are not commonly permissible for the user (uid)
  executing PHP.

- Issues that require privileges superior to those of the user (uid) executing
  PHP.

- The use of debugging facilities, e.g. Xdebug or `var_dump()`.

- The use of settings not recommended for production, e.g. error reporting to
  output.

- The use of non-standard environment variables, e.g. `USE_ZEND_ALLOC`.

- The use of non-standard builds, e.g. an obscure embedded platform or a
  compiler that is not commonly used.

- The use of legacy APIs or settings known to be insecure, particularly those
  documented as such, or those with a secure alternative.

- Insecure default arguments, such as a default algorithm selection (e.g.
  SHA1). Changing such defaults is handled through the deprecation process.

- The use of FFI.

- The use of the built-in web server, which is intended for development only.

- `open_basedir` or `disable_functions` bypasses.

- Malicious `unserialize()` inputs.

- Memory exhaustion from a size the input declares, where `memory_limit`
  refuses the allocation and only the current request dies.

- Bugs in third-party libraries, whether bundled (like lexbor/lexbor, libgd,
  libmagic, PCRE2 or timelib) or linked from the system (like
  libxml2, OpenSSL or libcurl). Please report them to the library's maintainers
  directly. Issues in how PHP uses these libraries remain in scope. The bundled
  libraries and their upstream locations are listed in
  [CONTRIBUTING.md](CONTRIBUTING.md#php-source-code-directory-structure).

- Issues that are present only in branches without a stable release. Such
  branches are per se not intended for production use.

If your report is classified as not being a security issue, it has nothing to do
with the bug being real or its importance to you. It just means it does not fit
our specific definitions for issues that we handle in a special way. Such issues
should be reported as regular bugs in the [issue
tracker](https://github.com/php/php-src/issues), and pull requests are always
welcome.

# Handling Security Issues

All security issues are handled through [GitHub security
advisories](https://github.com/php/php-src/security/advisories) in the
`php/php-src` repository. Each report is triaged by the PHP security team, which
confirms whether the issue is a security issue and assesses its severity.

Fixes for security issues are developed and reviewed in our private security
repository, and are merged into the public repository only when the release
containing them is tagged. A CVE identifier is assigned to each security issue
before the release, and the advisory is published together with the release.

Security fixes are shipped in a scheduled release (releases usually happen on
Thursdays). The time between the report and the release depends on the
complexity of the fix and the number of issues being handled at the time, so
please be patient. Fixes are released for all [supported
versions](https://www.php.net/supported-versions.php) affected by the issue, as
described in the [Release
Process](https://github.com/php/policies/blob/main/release-process.rst).

# Responsible Disclosure

Please report the issue as described above and communicate with the developers
about when the fix will be released. After the fix is released, please feel free
to disclose the issue as you see fit.

If you think an issue is a security issue but the developers disagree, please
read the classification above and try to explain to us why it fits the
description. Each bug usually has its aspects. If a short discussion does not
yield agreement, we would rather do more fixing and less arguing.

# Bounties

PHP is a volunteer project. We have no money, thus we can't pay bounties.
